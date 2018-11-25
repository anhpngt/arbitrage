#include "kucoin.h"

std::vector<std::string> Kucoin::SYMBOLS = {
    "BTC-USDT",
    "ETH-USDT",
    "LTC-USDT",
    "NEO-USDT"};

std::unordered_map<std::string, int> Kucoin::SYMBOL_MAP = {
    {"BTC-USDT", 0},
    {"ETH-USDT", 1},
    {"LTC-USDT", 2},
    {"NEO-USDT", 3}};

std::unordered_map<std::string, std::string> Kucoin::CHECK_MAP = {
    {"BTC", "BTC-USDT"},
    {"ETH", "ETH-USDT"},
    {"LTC", "LTC-USDT"},
    {"NEO", "NEO-USDT"}};

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