#include "binance.h"

std::vector<std::string> Binance::SYMBOLS = {
    "ETHBTC",
    "LTCBTC",
    "NEOBTC",
    "GASBTC",
    "WTCBTC",
    "QTUMBTC",
    "OMGBTC",
    "ZRXBTC",
    "KNCBTC",
    "SNMBTC"};

std::unordered_map<std::string, int> Binance::SYMBOL_MAP = {
    {"ETHBTC", 0},
    {"LTCBTC", 1},
    {"NEOBTC", 2},
    {"GASBTC", 3},
    {"WTCBTC", 4},
    {"QTUMBTC", 5},
    {"OMGBTC", 6},
    {"ZRXBTC", 7},
    {"KNCBTC", 8},
    {"SNMBTC", 9}};

std::unordered_map<std::string, std::string> Binance::CHECK_MAP = {
    {"ETC", "ETHBTC"},
    {"LTC", "LTCBTC"},
    {"NEO", "NEOBTC"},
    {"GAS", "GASBTC"},
    {"WTC", "WTCBTC"},
    {"QTUM", "QTUMBTC"},
    {"OMG", "OMGBTC"},
    {"ZRX", "ZRXBTC"},
    {"KNC", "KNCBTC"},
    {"SNM", "SNMBTC"},
};

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