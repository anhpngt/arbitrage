#ifndef API_H_
#define API_H_

#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <curl/curl.h>
#include <cpprest/ws_client.h>
#include <rapidjson/document.h>

// #define ENABLE_PROFILING

#ifdef ENABLE_PROFILING
#include <chrono>
#define PROFILER_BEGIN(name) auto PROFILE_MAIN_BEGIN_VAR_##name = std::chrono::system_clock::now()
#define PROFILER_END(name)                                                                        \
    {                                                                                             \
        auto PROFILE_MAIN_END_VAR_##name = std::chrono::system_clock::now();                      \
        auto PERIOD_##name = PROFILE_MAIN_END_VAR_##name - PROFILE_MAIN_BEGIN_VAR_##name;         \
        std::cout << "----- " << #name << " took: "                                               \
                  << std::chrono::duration_cast<std::chrono::milliseconds>(PERIOD_##name).count() \
                  << "ms" << std::endl;                                                           \
    }
#else
#define PROFILER_BEGIN(name)
#define PROFILER_END(name)
#endif // ENABLE_PROFILING

using std::cout;
using std::endl;

namespace arbitrage
{

class OrderBook
{
  public:
    // Price-Quantity pairs, key value: price level, mapped value: quantity
    std::unordered_map<double, double> asks;
    std::unordered_map<double, double> bids;

    // List of prices, sorted
    std::set<double> ask_prices, bid_prices;
};

class API
{
  public:
    API(const std::string &exchange_name);
    ~API();

    /* Use REST API to get quotes. API is implemented in class' constructor */
    bool requestRestApi(const std::string &url);

    /* Parse price from json */
    // virtual void parsePrice(std::vector<double> &sell_price, std::vector<double> &buy_price) = 0;

    /* Get member values */
    // inline int getHttpResult() const { return http_code_; }
    inline const std::string &name() const { return exchange_name_; }
    inline const std::string &httpData() const { return http_data_; }

    /** Pretty print the order book */
    void printOrderBook(const size_t coin_idx);

    static constexpr double FEE_TRADE_RATIO = 0.001;

  protected:
    virtual void updateOrderBookCallback(const web::web_sockets::client::websocket_incoming_message &msg) = 0;

    /** Name of the exchange */
    std::string exchange_name_;

    /** Order book map */
    std::vector<OrderBook> order_book_;

    /** Variables for websocket */
    web::websockets::client::websocket_callback_client client_;

    /** Json parser, is refilled after every requestRestApi() call */
    rapidjson::Document document_;

    std::mutex order_book_mutex_; // to handle order book

  private:
    void release();

    /** Callback function for REST API */
    static std::size_t writeData(const char *in, std::size_t size, std::size_t num, std::string *out);

    /** Variables for REST API */
    CURL *curl_;                 // curl
    struct curl_slist *headers_; // out header specs
    CURLcode res_;               // retrieval curl result
    std::string http_data_;      // retrieval raw json data
    int http_code_;              // retrieval result code
};

} // namespace arbitrage

#endif