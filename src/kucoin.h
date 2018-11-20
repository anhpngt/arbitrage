#ifndef KUCOIN_H_
#define KUCOIN_H_

#include <unordered_map>
#include <vector>

#include "api.h"

class Kucoin : public API
{
  public:
    Kucoin()
    {
        // curl_easy_setopt(curl_, CURLOPT_URL, "https://api.kucoin.com/v1/open/tick?symbol=ETH-BTC");
        curl_easy_setopt(curl_, CURLOPT_URL, "https://api.kucoin.com/v1/open/tick");
    }

    static std::vector<std::string> SYMBOLS;
    static std::unordered_map<std::string, int> SYMBOL_MAP;
    static std::unordered_map<std::string, std::string> CHECK_MAP;

    void parsePrice(std::vector<double> &sell_price, std::vector<double> &buy_price);
};

#endif