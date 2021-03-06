#include <time.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "Producer.h"

// TODO: This thread should tell the main thread whether or not it received an empty page, signalling we have searched all
// pages. Then the main thread can stop producing and allow the consumers to die off when they finish.


/**** PRODUCER MEMBER FUNCTIONS ****/

/*
 * Producer constructor function
 * Sets the link and page number variables
 * as well as the time last checked from the setLastChecked()
 * function.
 */
Producer::Producer(std::string link, int page_num, std::queue<std::string> *buffer, Semaphore *items, std::queue<int> *pids, bool *empty_page_reached) {
  this->link = link;
  this->page_num = page_num;
  this->buffer = buffer;
  this->items = items;
  this->pids = pids;
  this->empty_page_reached = empty_page_reached;
  setLastChecked();
}

/*
 * Sets the curl options.
 * Returns false if the curl handle is invalid.
 */
bool Producer::setCurlOptions(void) {
  if(curl) {
    // Set URL and callbacks
    curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buf);

    // Shopify will block request without useragent
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:72.0) Gecko/20100101 Firefox/72.0");

    // Follow redirects
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    return true;
  }
  return false;
}

/*
 * Performs the request and calls parseJSON
 * to parse the result.
 * Also creates and cleans up the curl handle and
 * calls setCurlOptions.
 * Returns the curl response code.
 */
int Producer::perform(void) {
  CURLcode res;

  // Setup curl
  curl = curl_easy_init();

  // Set curl options
  setCurlOptions();

  // Send the request
  res = curl_easy_perform(curl);

  // Cleanup after request
  curl_easy_cleanup(curl);

  if (res == CURLE_OK) {
    // Parse what was read if
    // received good response
    parseJSON();
  }

  pids->push(page_num);
  return res;
}

/* 
 * Returns page number assigned to this instance
 */
int Producer::getPageNum(void) {
  return page_num;
}

/* 
 * Returns link assigned to this instance
 * Note that many instances will have the same link
 * since they are scraping multiple pages of the same
 * site
 */
std::string Producer::getLink(void) {
  return link;
}

/*
 * Sets the last time checked for an update
 * A dummy value is used here for testing atm
 * Mayube should read this from a file
 */
void Producer::setLastChecked(void) {
  struct tm tm;
  strptime("2020-01-13T0:00:00-05:00", "%Y-%m-%dT%T", &tm);
  last_checked = mktime(&tm);
}

/*
 * The write callback is called when we get a response from the
 * server. It handles the response.
 * Returns the number of bytes received.
 */
size_t Producer::writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
  // Receive data from server here
  ((std::string*)userdata)->append((char*)ptr, size*nmemb);
  return size*nmemb;
}

/*
 * Parses the JSON result received from the server and
 * saves updates into the results buffer.
 * TODO: We should also be handling errors here. In case
 * there is no "products" top level attribute.
 */
int Producer::parseJSON(void) {
  rapidjson::Document out_json;
  rapidjson::Document d;

  // Parse top level JSON
  d.Parse(read_buf.c_str());

  if (d["products"].GetArray().Empty())
    *empty_page_reached = true;

  // Loop through products array and store the ones we want
  // in a vector
  for (const auto& product : d["products"].GetArray()) {
    struct tm tm_p;
    struct tm tm_u;
    time_t t_p;
    time_t t_u;

    std::string pub = product["published_at"].GetString();
    std::string upd = product["updated_at"].GetString();

    // 2020-01-11T10:00:02-05:00 => ISO format
    strptime(pub.c_str(), "%Y-%m-%dT%T", &tm_p);
    strptime(upd.c_str(), "%Y-%m-%dT%T", &tm_u);

    t_p = mktime(&tm_p);
    t_u = mktime(&tm_u);

    // If published after last checked or updated after last checked
    if (difftime(t_p, last_checked) >= 0) {
      rapidjson::StringBuffer out_buf;
      rapidjson::Writer<rapidjson::StringBuffer> writer(out_buf);
      product.Accept(writer);

      // Push item to buffer and increment semaphore
      buffer->push(out_buf.GetString());
      items->post();
    } else if (difftime(t_u, last_checked) >= 0) {
      rapidjson::StringBuffer out_buf;
      rapidjson::Writer<rapidjson::StringBuffer> writer(out_buf);
      product.Accept(writer);

      // Push item to buffer and increment semaphore
      buffer->push(out_buf.GetString());
      items->post();
    }
  }

  // Always return OK for now
  return 0;
}

/*
 * The read callback is for sending a request to the server.
 * We are not sending any requests atm so this is not needed.
 */
// size_t read_callback(char *buffer, size_t size, size_t nitems, void *inputdata) {
//   // Write data to server here
// }
