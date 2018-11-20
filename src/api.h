#ifndef API_H_
#define API_H_

#include <iostream>
#include <iomanip>
#include <memory>
#include <string>

#include <curl/curl.h>
#include <json/json.h>

// #define ENABLE_PROFILING
#ifdef ENABLE_PROFILING
#include <chrono>
#include <stdio.h>
#define PROFILE_MAIN_BEGIN(name) auto PROFILE_MAIN_BEGIN_VAR_##name = std::chrono::high_resolution_clock::now()
#define PROFILE_MAIN_END(name)                                                                                                              \
    {                                                                                                                                       \
        auto PROFILE_MAIN_END_VAR_##name = std::chrono::high_resolution_clock::now();                                                       \
        auto PERIOD_##name = PROFILE_MAIN_END_VAR_##name - PROFILE_MAIN_BEGIN_VAR_##name;                                                   \
        printf("%s%s took %.6fms\n", "PROFILE_MAIN_", #name, std::chrono::duration_cast<std::chrono::milliseconds>(PERIOD_##name).count()); \
    }
#else
#define PROFILE_MAIN_BEGIN(name)
#define PROFILE_MAIN_END(name)
#endif // ENABLE_PROFILING

using namespace std;

class API
{
  public:
    API();
    ~API();

    bool requestPrice();
    void printResult() { cout << json_data_.toStyledString() << endl; }
    std::string getRequestResult() const { return json_data_.toStyledString(); }
    virtual void parsePrice(std::vector<double> &sell_price, std::vector<double> &buy_price) = 0;

    const double FEE_TRADE_RATIO = 0.001;

  protected:
    CURL *curl_;
    struct curl_slist *headers_;
    std::string http_data_;

    CURLcode res_;
    int http_code_;

    Json::Value json_data_;
    Json::Reader json_reader_;

  private:
    void release();
    static std::size_t writeData(const char *in, std::size_t size, std::size_t num, std::string *out);
};

#endif