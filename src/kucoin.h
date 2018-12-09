#ifndef KUCOIN_H_
#define KUCOIN_H_

#include <atomic>
#include <unordered_map>
#include <vector>

#include "api.h"

namespace arbitrage
{

class Kucoin : public API
{
  public:
    Kucoin();

  private:
    /** Fill up the order book with data */
    void intializeOrderBook();

    /** Update order book after it has been filled */
    void updateOrderBookCallback(const web::web_sockets::client::websocket_incoming_message &msg);

    /** Helper function to add data to order book */
    void updateBook(OrderBook &book, const rapidjson::Value &asks, const rapidjson::Value &bids);

    bool is_initialized_;  // is all order books initialized
    size_t list_size_;     // number of coins monitored
    bool is_ws_connected_; // is connect to Kucoin websocket
    std::mutex init_mutex_;

    std::string bullet_token_;
    std::string ws_base_endpoint_;

    /** Constant variables for Kucoin's paths */
    static const std::vector<std::string> SYMBOLS;
    static const std::unordered_map<std::string, int> SYMBOL_MAP;
    static const std::unordered_map<std::string, std::string> CHECK_MAP;
    static const std::vector<std::string> TOPIC_NAMES;

    static const std::string RESTAPI_BASE_ENDPOINT;
    static const std::string RESTAPI_ORDER_BOOK_ENDPOINT;
    static const std::string WS_BASE_ENDPOINT; // use ws_base_endpoint_ instead, which is dynamically obtained
    static const int ORDER_BOOK_LIMIT;

    static const std::string WEBSOCKET_REQUEST_ENDPOINT;

    // void parsePrice(std::vector<double> &sell_price, std::vector<double> &buy_price);
};

} // namespace arbitrage

#endif