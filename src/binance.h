#ifndef BINANCE_H_
#define BINANCE_H_

#include <algorithm>
#include <unordered_map>
#include <vector>

#include "api.h"

namespace arbitrage
{

class Binance : public API
{
  public:
    /** Constructor */
    Binance();

  private:
    /** Fill up the order book with data */
    void intializeOrderBook();

    /** Update order book after it has been filled */
    void updateOrderBookCallback(const web::web_sockets::client::websocket_incoming_message &msg);

    /** Helper function to add data to order book */
    void updateBook(OrderBook &book, const rapidjson::Value &asks, const rapidjson::Value &bids);

    bool is_initialized_;               // is all order books initialized
    std::vector<bool> is_first_update_; // is each order book's first update? Check sequence number validity
    size_t list_size_;                  // number of coins monitored
    std::vector<int> last_update_ids_;  // lastUpdateIds to check sequence validity
    std::mutex init_mutex_;

    /** Constant variables for Binance's paths */
    static const std::vector<std::string> SYMBOLS;
    static const std::unordered_map<std::string, int> SYMBOL_MAP;
    static const std::unordered_map<std::string, std::string> CHECK_MAP;
    static const std::vector<std::string> STREAM_NAMES;

    static const std::string RESTAPI_BASE_ENDPOINT;
    static const std::string RESTAPI_ORDER_BOOK_ENDPOINT;
    static const std::string WS_BASE_ENDPOINT;
    static const int ORDER_BOOK_LIMIT;
};

} // namespace arbitrage

#endif