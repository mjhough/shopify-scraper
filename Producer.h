#include <iostream>
#include <string>
#include <queue>

#include <curl/curl.h>

class Producer {
  public:
    // Constructor
    Producer(std::string link, int page_num);

    // Instance variables
    std::string link;                                                                         // Link that will be scraped
    int page_num;                                                                             // Page number of JSON
    int perform();
    time_t last_checked;                                                                      // Time last checked for updates
    bool setCurlOptions();
    
    // Member functions
    int getPageNum();                                                                         // Gets the page number of the producer instance
    std::string getLink();                                                                     // Gets the link of the producer instance
    void setLastChecked();                                                                    // Gets the time the producer instance last checked for updates

    // Class functions
    static void printResults();                                                               // Print the JSON strings from the results vector

  private:
    // Instance variables
    std::string read_buf;                                                                     // Buffer to read in results from server
    CURL *curl;

    // Class variables (static members)
    static std::queue<std::string> results;                                                   // Producer buffer. Will probably need to change from private later so
                                                                                              // that the Consumer can read from it. May need superclass for both.
                                                                                              // Call it "Worker" or something.

    // Functions
    int parseJSON();                                                                          // Parses JSON result

    // Static functions
    static size_t writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata);               // Libcurl write callback - handles data received from server
};
