#include "kucoin.h"

std::vector<std::string> Kucoin::SYMBOLS = {
    "ETH-BTC",
    "LTC-BTC",
    "NEO-BTC",
    "GAS-BTC",
    "WTC-BTC",
    "QTUM-BTC",
    "OMG-BTC",
    "ZRX-BTC",
    "KNC-BTC",
    "SNM-BTC"};

std::unordered_map<std::string, int> Kucoin::SYMBOL_MAP = {
    {"ETH-BTC", 0},
    {"LTC-BTC", 1},
    {"NEO-BTC", 2},
    {"GAS-BTC", 3},
    {"WTC-BTC", 4},
    {"QTUM-BTC", 5},
    {"OMG-BTC", 6},
    {"ZRX-BTC", 7},
    {"KNC-BTC", 8},
    {"SNM-BTC", 9}};

std::unordered_map<std::string, std::string> Kucoin::CHECK_MAP = {
    {"ETC", "ETH-BTC"},
    {"LTC", "LTC-BTC"},
    {"NEO", "NEO-BTC"},
    {"GAS", "GAS-BTC"},
    {"WTC", "WTC-BTC"},
    {"QTUM", "QTUM-BTC"},
    {"OMG", "OMG-BTC"},
    {"ZRX", "ZRX-BTC"},
    {"KNC", "KNC-BTC"},
    {"SNM", "SNM-BTC"},
};

void Kucoin::parsePrice(std::vector<double> &sell_price, std::vector<double> &buy_price)
{
    sell_price.resize(SYMBOLS.size());
    buy_price.resize(SYMBOLS.size());
    if (json_data_["success"].asBool())
    {
        for (auto &item : json_data_["data"])
        {
            if (item.isMember("buy") && item.isMember("sell"))
            {
                std::unordered_map<std::string, int>::const_iterator look_up = SYMBOL_MAP.find(item["symbol"].asString());
                if (look_up != SYMBOL_MAP.end())
                {
                    sell_price[look_up->second] = std::stod(item["sell"].asString());
                    buy_price[look_up->second] = std::stod(item["buy"].asString());
                }
            }
        }
    }
    else
        throw std::runtime_error("Success not true in response");
}