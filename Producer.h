#include <curl/curl.h>
#include <queue>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "Worker.h"

class Producer : protected Worker {
  public:
    // Constructor
    Producer(std::string link, int page_num, std::queue<std::string> *buffer, Semaphore *items, std::queue<int> *pids, bool *empty_page_reached);

    // Instance variables
    std::string link;                                                                         // Link that will be scraped
    int perform();
    time_t last_checked;                                                                      // Time last checked for updates
    bool setCurlOptions();
    
    // Member functions
    int getPageNum();                                                                         // Gets the page number of the producer instance
    std::string getLink();                                                                     // Gets the link of the producer instance
    void setLastChecked();                                                                    // Gets the time the producer instance last checked for updates

  private:
    // Instance variables
    std::string read_buf;                                                                     // Buffer to read in results from server
    CURL *curl;
    int page_num;                                                                             // Page number of JSON
    bool *empty_page_reached;
    std::queue<int> *pids;

    // Functions
    int parseJSON();                                                                          // Parses JSON result

    // Static functions
    static size_t writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata);        // Libcurl write callback - handles data received from server
};
