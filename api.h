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

class Binance : public API
{
  public:
    Binance()
    {
        // curl_easy_setopt(curl_, CURLOPT_URL, "https://api.binance.com/api/v3/ticker/price?symbol=ETHBTC");
        curl_easy_setopt(curl_, CURLOPT_URL, "https://api.binance.com/api/v3/ticker/price");
    }

    void parse(double &buy_price, double &sell_price)
    {
        buy_price = std::stod(json_data_["price"].asString());
        sell_price = buy_price;
    }
};

class Kucoin : public API
{
  public:
    Kucoin()
    {
        // curl_easy_setopt(curl_, CURLOPT_URL, "https://api.kucoin.com/v1/open/tick?symbol=ETH-BTC");
        curl_easy_setopt(curl_, CURLOPT_URL, "https://api.kucoin.com/v1/open/tick");
    }

    void parse(double &buy_price, double &sell_price)
    {
        buy_price = json_data_["data"]["buy"].asDouble();
        sell_price = json_data_["data"]["sell"].asDouble();
    }
};

#endif