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

    // void parsePrice(std::vector<double> &sell_price, std::vector<double> &buy_price);

  private:
    /** Fill up the order book with data */
    void intializeOrderBook();

    /** Update order book after it has been filled */
    void updateOrderBookCallback(const web::web_sockets::client::websocket_incoming_message &msg);
    void updateBook(OrderBook &book, const rapidjson::Value &asks, const rapidjson::Value &bids);

    bool is_initialized_;
    std::vector<bool> is_first_update_;
    size_t list_size_;
    std::vector<int> last_update_ids_;
    std::mutex init_mutex_;

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