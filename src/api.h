#ifndef API_H_
#define API_H_

#include <iostream>
#include <iomanip>
#include <memory>
#include <string>

#include <curl/curl.h>
#include <json/json.h>

using namespace std;

class API
{
  public:
    API();
    ~API();

    bool request();
    void printResult() { cout << json_data_.toStyledString() << endl; }
    virtual void parse(double &buy_price, double &sell_price) = 0;

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