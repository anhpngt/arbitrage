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
    for (auto &item : json_data_)
    {
        if (item.isMember("price") && item.isMember("symbol"))
        {
            std::unordered_map<std::string, int>::const_iterator look_up = SYMBOL_MAP.find(item["symbol"].asString());
            if (look_up != SYMBOL_MAP.end())
            {
                sell_price[look_up->second] = std::stod(item["price"].asString());
            }
        }
        else
            throw std::runtime_error(item.toStyledString());
    }

    buy_price = sell_price;
}