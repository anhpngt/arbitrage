#ifndef API_H_
#define API_H_

#include <iostream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <curl/curl.h>
#include "rapidjson/document.h"

// #define ENABLE_PROFILING
#ifdef ENABLE_PROFILING
#include <chrono>
#define PROFILER_BEGIN(name) auto PROFILE_MAIN_BEGIN_VAR_##name = std::chrono::system_clock::now()
#define PROFILER_END(name)                                                                        \
    {                                                                                             \
        auto PROFILE_MAIN_END_VAR_##name = std::chrono::system_clock::now();                      \
        auto PERIOD_##name = PROFILE_MAIN_END_VAR_##name - PROFILE_MAIN_BEGIN_VAR_##name;         \
        std::cout << "----- " << #name << " took: "                                               \
                  << std::chrono::duration_cast<std::chrono::milliseconds>(PERIOD_##name).count() \
                  << "ms" << std::endl;                                                           \
    }
#else
#define PROFILER_BEGIN(name)
#define PROFILER_END(name)
#endif // ENABLE_PROFILING

using namespace std;

class API
{
  public:
    API();
    ~API();

    /* Use REST API to get quotes. API is implemented in class' constructor */
    bool requestPrice();

    /* Parse price from json */
    virtual void parsePrice(std::vector<double> &sell_price, std::vector<double> &buy_price) = 0;

    /* Get member values */
    inline int getHttpResult() const { return http_code_; }
    inline std::string name() const { return name_; }

    const double FEE_TRADE_RATIO = 0.001;

  protected:
    std::string name_;
    CURL *curl_;
    struct curl_slist *headers_;
    std::string http_data_;

    CURLcode res_;
    int http_code_;

    rapidjson::Document document_;

  private:
    void release();
    static std::size_t writeData(const char *in, std::size_t size, std::size_t num, std::string *out);
};

#endif