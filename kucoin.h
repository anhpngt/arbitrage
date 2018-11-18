#ifndef KUCOIN_H_
#define KUCOIN_H_

#include <iostream>
#include <iomanip>
#include <memory>
#include <string>

#include <curl/curl.h>
#include <json/json.h>

using namespace std;

class Kucoin
{
  public:
    Kucoin();
    Kucoin(const std::string &base_url, const std::string &coin);
    ~Kucoin();

    void release();

    bool get();

  private:
    static std::size_t writeData(const char *in, std::size_t size, std::size_t num, std::string *out);

    CURL *curl_;
    std::string base_url_;
    std::string coin_;
    struct curl_slist *headers_;
    std::unique_ptr<std::string> http_data_;

    CURLcode res_;
    int http_code_;

    Json::Value json_data_;
    Json::Reader json_reader_;
};

#endif