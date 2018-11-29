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
    if (document_.HasMember("success"))
    {
        if (document_["success"].GetBool())
        {
            for (const auto &item : document_["data"].GetArray())
            {
                if (item.HasMember("buy") && item.HasMember("sell"))
                {
                    std::unordered_map<std::string, int>::const_iterator look_up = SYMBOL_MAP.find(item["symbol"].GetString());
                    if (look_up != SYMBOL_MAP.end())
                    {
                        sell_price[look_up->second] = item["sell"].GetDouble();
                        buy_price[look_up->second] = item["buy"].GetDouble();
                    }
                }
            }
        }
        else
            throw std::runtime_error("[Kucoin] Success not true in response");
    }
    else
        throw std::runtime_error("[Kucoin] Invalid json response");
}