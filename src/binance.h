#ifndef BINANCE_H_
#define BINANCE_H_

#include "api.h"

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

#endif