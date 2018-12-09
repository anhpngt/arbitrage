#ifndef KUCOIN_H_
#define KUCOIN_H_

#include "api.h"

namespace arbitrage
{

class Kucoin : public API
{
  public:
    Kucoin();

    /** List size */
    inline int getSize() const { return list_size_; }

    /** Status */
    inline bool getStatus() const { return is_initialized_; }

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
    static const int PING_INTERVAL; // in ms

  private:
    /** Fill up the order book with data */
    void intializeOrderBook();

    /** Update order book after it has been filled */
    void updateOrderBookCallback(const web::web_sockets::client::websocket_incoming_message &msg);

    /** Helper function to add data to order book */
    void updateBook(OrderBook &book, const rapidjson::Value &asks, const rapidjson::Value &bids);

    /** Ping the server periodically to keep the connection alive */
    void pingServer();

    bool is_initialized_;  // is all order books initialized
    size_t list_size_;     // number of coins monitored
    bool is_ws_connected_; // is connect to Kucoin websocket
    std::mutex init_mutex_;

    std::string bullet_token_;
    std::string ws_base_endpoint_;

    std::thread pinger_thread_;
    web::web_sockets::client::websocket_outgoing_message msg_ping_;

    // void parsePrice(std::vector<double> &sell_price, std::vector<double> &buy_price);
};

} // namespace arbitrage

#endif