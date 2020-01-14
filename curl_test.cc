#include <iostream>
#include <string>
#include <vector>

#include <curl/curl.h>
#include <time.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

int parse_json(const char* buf);
std::string doc_to_json_str(rapidjson::Document);

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
  // Receive data from server here
  ((std::string*)userdata)->append((char*)ptr, size*nmemb);
  return size*nmemb;
}

// size_t read_callback(char *buffer, size_t size, size_t nitems, void *inputdata) {
//   // Write data to server here
// }

int main(void) {
  CURL *curl;
  CURLcode res;
  std::string read_buf;

  curl = curl_easy_init();
  if(curl) {
    // Set URL and callbacks
    curl_easy_setopt(curl, CURLOPT_URL, "http://kith.com/products.json?page=1");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buf);

    // Shopify will block request without useragent
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:72.0) Gecko/20100101 Firefox/72.0");

    // Follow redirects
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);



    // Send the request
    res = curl_easy_perform(curl);

    // Cleanup after request
    curl_easy_cleanup(curl);

    // Print out what was read
    parse_json(read_buf.c_str());
  }
  return 0;
}

int parse_json(const char* buf) {
  rapidjson::Document out_json;
  rapidjson::Document::AllocatorType& alctr = out_json.GetAllocator();
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

    if (difftime(t_p, t_u) >= 0) {
      std::cout << "Last update was <= publishing." << std::endl;
    } else {
      std::cout << "Updated after publishing." << std::endl;
      rapidjson::StringBuffer out_buf;
      rapidjson::Writer<rapidjson::StringBuffer> writer(out_buf);
      product.Accept(writer);
      
      rapidjson::Value out_val;
      std::string out_str = out_buf.GetString();
      out_val.SetString(out_str.c_str(), out_str.length(), alctr);
      out_json.PushBack(out_val, alctr);
    }
  }

  // Convert document to JSON string and print
  rapidjson::StringBuffer sb;
  rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
  out_json.Accept(writer);
  std::cout << sb.GetString() << std::endl;

  // Always return OK for now
  return 0;
}
