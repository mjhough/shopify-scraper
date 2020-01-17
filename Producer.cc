#include <iostream>
#include <string>
#include <vector>

#include <curl/curl.h>
#include <time.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

class Producer {
  public:
    // Constructor
    Producer(std::string link, int page_num);

    // Instance variables
    std::string link;                                                                         // Link that will be scraped
    int page_num;                                                                             // Page number of JSON
    time_t last_checked;                                                                      // Time last checked for updates
    
    // Class variables (static members)
    static std::vector<std::string> results;
    
    // Member functions
    int getPageNum();                                                                         // Gets the page number of the producer instance
    std::string getLink()                                                                     // Gets the link of the producer instance
    void getLastChecked();                                                                    // Gets the time the producer instance last checked for updates

  private:
    // Instance variables
    std::string read_buf;

    // Functions
    int parseJSON(const char* buf);                                                           // Parses JSON result
    size_t writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata);               // Libcurl write callback - handles data received from server
};

Producer::Producer(std::string link, int page_num) {
  this->link = link;
  this->page_num = page_num;
  this->last_checked = getLastChecked();
}

int Producer::getPageNum(void) {
  return page_num;
}

std::string Producer::getLink(void) {
  return link;
}

// Dummy function for testing to set last checked to a hardcoded value
// Maybe should actually read this from a file
void Producer::getLastChecked(void) {
  struct tm tm;
  strptime("2020-01-13T0:00:00-05:00", "%Y-%m-%dT%T", &tm);
  last_checked = mktime(&tm);
}

size_t Producer::writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
  // Receive data from server here
  ((std::string*)userdata)->append((char*)ptr, size*nmemb);
  return size*nmemb;
}

int Producer::parseJSON(const char* buf) {
  rapidjson::Document out_json;
  rapidjson::Document d;

  // Parse top level JSON
  d.Parse(buf);

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
      std::cout << "New product published since last checked." << std::endl;
      rapidjson::StringBuffer out_buf;
      rapidjson::Writer<rapidjson::StringBuffer> writer(out_buf);
      product.Accept(writer);
      results.push_back(out_buf.GetString());
    } else if (difftime(t_u, last_checked) >= 0) {
      std::cout << "Existing product updated since last checked." << std::endl;
      rapidjson::StringBuffer out_buf;
      rapidjson::Writer<rapidjson::StringBuffer> writer(out_buf);
      product.Accept(writer);
      results.push_back(out_buf.GetString());
    }
  }

  for (int i =0; i < results.size(); i++) {
    std::cout << results[i] << std::endl;
  }

  // Always return OK for now
  return 0;
}

// TODO: Turn this file into a class called producer or something
//  - Each producer will be initialized with a link (and page number) and the buffer it produces to. Maybe have a class variable for producer buffer?
//  - Each producer will have its own thread and will request a page from the site.
//  - Each producer will store the results as JSON in the producer buffer.
//  - The producer buffer can be implemented as a queue so that the consumer can read off of it.
//  - The consumer will take items off of the queue and process them, probably sending curl requests to a discord server or something.



// size_t read_callback(char *buffer, size_t size, size_t nitems, void *inputdata) {
//   // Write data to server here
// }

bool Producer::setCurlOptions(void) {
  if(curl) {
    // Set URL and callbacks
    curl_easy_setopt(curl, CURLOPT_URL, "http://kith.com/products.json?page=1");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buf);

    // Shopify will block request without useragent
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:72.0) Gecko/20100101 Firefox/72.0");

    // Follow redirects
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    return true;
  }
  return false;
}


int Producer::perform(void) {
  CURLcode res;
  CURL *curl

  // Setup curl
  curl = curl_easy_init();

  // Send the request
  res = curl_easy_perform(curl); // Good here

  // Cleanup after request
  curl_easy_cleanup(curl);

  // Parse what was read
  parseJSON(read_buf.c_str());

  return res;
}
