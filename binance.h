#ifndef BINANCE_H_
#define BINANCE_H_

#include <iostream>
#include <iomanip>
#include <memory>
#include <string>

#include <curl/curl.h>
#include <json/json.h>

using namespace std;

class Binance
{
  public:
    Binance();
    Binance(const std::string &base_url, const std::string &coin);
    ~Binance();

    void release();

    bool get();

  private:
    static std::size_t writeData(const char *in, std::size_t size, std::size_t num, std::string *out);

    CURL *curl_;
    std::string base_url_;
    std::string coin_;
    struct curl_slist *headers_;
    std::string http_data_;

    CURLcode res_;
    int http_code_;

    Json::Value json_data_;
    Json::Reader json_reader_;
};

#endif