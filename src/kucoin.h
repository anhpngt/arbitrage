#ifndef KUCOIN_H_
#define KUCOIN_H_

#include "api.h"

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