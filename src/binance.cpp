#include "binance.h"

std::vector<std::string> Binance::SYMBOLS = {
    "BTCUSDT",
    "ETHUSDT",
    "LTCUSDT",
    "NEOUSDT"};

std::unordered_map<std::string, int> Binance::SYMBOL_MAP = {
    {"BTCUSDT", 0},
    {"ETHUSDT", 1},
    {"LTCUSDT", 2},
    {"NEOUSDT", 3}};

std::unordered_map<std::string, std::string> Binance::CHECK_MAP = {
    {"BTC", "BTCUSDT"},
    {"ETH", "ETHUSDT"},
    {"LTC", "LTCUSDT"},
    {"NEO", "NETUSDT"}};

void Binance::parsePrice(std::vector<double> &sell_price, std::vector<double> &buy_price)
{
    sell_price.resize(SYMBOLS.size());
    buy_price.resize(SYMBOLS.size());
    for (const auto &item : document_.GetArray())
    {
        if (item.HasMember("symbol") && item.HasMember("bidPrice") && item.HasMember("askPrice"))
        {
            std::unordered_map<std::string, int>::const_iterator look_up = SYMBOL_MAP.find(item["symbol"].GetString());
            if (look_up != SYMBOL_MAP.end())
            {
                sell_price[look_up->second] = std::stod(item["askPrice"].GetString());
                buy_price[look_up->second] = std::stod(item["bidPrice"].GetString());
            }
        }
        else
            throw std::runtime_error("[Binance] Invalid item encountered");
    }
}