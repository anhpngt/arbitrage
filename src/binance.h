#ifndef BINANCE_H_
#define BINANCE_H_

#include <unordered_map>
#include <vector>

#include "api.h"

class Binance : public API
{
  public:
    Binance()
    {
        name_ = "Binance";
        curl_easy_setopt(curl_, CURLOPT_URL, "https://api.binance.com/api/v3/ticker/price");
    }

    static std::vector<std::string> SYMBOLS;
    static std::unordered_map<std::string, int> SYMBOL_MAP;
    static std::unordered_map<std::string, std::string> CHECK_MAP;

    void parsePrice(std::vector<double> &sell_price, std::vector<double> &buy_price);
};

#endif