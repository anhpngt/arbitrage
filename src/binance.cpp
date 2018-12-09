#include "binance.h"

namespace arbitrage
{

const std::vector<std::string> Binance::SYMBOLS = {
    "BTCUSDT",
    "ETHUSDT",
    "LTCUSDT",
    "NEOUSDT"};

const std::unordered_map<std::string, int> Binance::SYMBOL_MAP = {
    {"BTCUSDT", 0},
    {"ETHUSDT", 1},
    {"LTCUSDT", 2},
    {"NEOUSDT", 3}};

const std::unordered_map<std::string, std::string> Binance::CHECK_MAP = {
    {"BTC", "BTCUSDT"},
    {"ETH", "ETHUSDT"},
    {"LTC", "LTCUSDT"},
    {"NEO", "NETUSDT"}};

const std::vector<std::string> Binance::STREAM_NAMES = {
    "btcusdt@depth",
    "ethusdt@depth",
    "ltcusdt@depth",
    "neousdt@depth"};

const std::string Binance::RESTAPI_BASE_ENDPOINT = "https://api.binance.com";
const std::string Binance::RESTAPI_ORDER_BOOK_ENDPOINT = "/api/v1/depth";
const std::string Binance::WS_BASE_ENDPOINT = "wss://stream.binance.com:9443";
const int Binance::ORDER_BOOK_LIMIT = 100;

Binance::Binance()
    : API("Binance"), is_initialized_(false), is_first_update_(SYMBOLS.size(), false), list_size_(SYMBOLS.size())
{
    // Connect to websocket
    std::string stream_name = WS_BASE_ENDPOINT + "/stream?streams=";
    for (const std::string &name : STREAM_NAMES)
    {
        stream_name += name + "/";
    }
    std::cout << "[Binance] Combined stream name:\n\t" << stream_name << std::endl;
    client_.connect(U(stream_name)).wait();

    // Query all orderbook data
    intializeOrderBook();
}

void Binance::intializeOrderBook()
{
    std::lock_guard<std::mutex> lck(init_mutex_);
    if (is_initialized_)
        return;

    std::cout << "[Binance] Initializing order book" << std::endl;

    order_book_.clear();
    order_book_.resize(list_size_);
    last_update_ids_.resize(list_size_);
    for (size_t i = 0, size = list_size_; i < size; ++i)
    {
        // Request until success
        std::string restapi_url = RESTAPI_BASE_ENDPOINT + RESTAPI_ORDER_BOOK_ENDPOINT + "?symbol=" + SYMBOLS[i] + "&limit=" + std::to_string(ORDER_BOOK_LIMIT);
        while (!requestRestApi(restapi_url))
            ;

        if (!(document_.HasMember("lastUpdateId") && document_.HasMember("asks") && document_.HasMember("bids")))
            throw std::runtime_error("[Binance] Orderbook init error: missing queries in JSON data");

        // Fill up order book with data from json parser
        last_update_ids_[i] = document_["lastUpdateId"].GetInt();
        updateBook(order_book_[i], document_["asks"], document_["bids"]);
    }

    is_initialized_ = true;
    is_first_update_ = std::vector<bool>(list_size_, true);
}

void Binance::updateOrderBookCallback(const web::web_sockets::client::websocket_incoming_message &msg)
{
    if (!is_initialized_)
    {
        cout << "Received callback but not init yet" << endl;
        intializeOrderBook();
        return;
    }

    // Parse JSON
    rapidjson::Document parser;
    parser.Parse(msg.extract_string().get().c_str());
    if (parser.HasParseError())
    {
        is_initialized_ = false;
    }

    // Check if json has the required keys
    if (!(parser.HasMember("stream") && parser.HasMember("data")))
    {
        return;
    }

    // Which stream?
    auto lookup = std::find(STREAM_NAMES.begin(), STREAM_NAMES.end(), parser["stream"].GetString());
    if (lookup == STREAM_NAMES.end())
        throw std::runtime_error(std::string("Cannot find matched stream name of: ") + parser["stream"].GetString());

    size_t list_idx = lookup - STREAM_NAMES.begin();
    if (is_first_update_[list_idx])
    {
        // Check first processed should have U <= lastUpdateId+1 AND u >= lastUpdateId+1
        // step 5 in https://github.com/binance-exchange/binance-official-api-docs/blob/master/web-socket-streams.md#how-to-manage-a-local-order-book-correctly
        int idplus1 = last_update_ids_[list_idx] + 1;
        if (parser["data"]["u"].GetInt() < idplus1)
        {
            cout << "Dropped data" << endl;
            return;
        }
        if (!(parser["data"]["U"].GetInt() <= idplus1 && parser["data"]["u"].GetInt() >= idplus1))
        {
            cout << idplus1 << endl;
            cout << parser["data"]["U"].GetInt() << " and " << parser["data"]["u"].GetInt() << endl;
            is_initialized_ = false; // re-init if failed
            std::cout << "[Binance] ERROR: First order book update failed" << std::endl;
        }

        is_first_update_[list_idx] = false;
    }

    // Update order book
    updateBook(order_book_[list_idx], parser["data"]["a"], parser["data"]["b"]);
}

void Binance::updateBook(OrderBook &book, const rapidjson::Value &asks, const rapidjson::Value &bids)
{
    assert(asks.IsArray());
    assert(bids.IsArray());
    std::lock_guard<std::mutex> ob_lck(order_book_mutex_);

    for (const auto &data : asks.GetArray())
    {
        double price_val = std::stod(data[0].GetString());
        double quantity_val = std::stod(data[1].GetString());
        if (quantity_val > 0.0)
        {
            book.asks[price_val] = quantity_val;
            book.ask_prices.insert(price_val);
        }
        else
        {
            book.asks.erase(price_val);
            book.ask_prices.erase(price_val);
        }
    }
    for (const auto &data : bids.GetArray())
    {
        double price_val = std::stod(data[0].GetString());
        double quantity_val = std::stod(data[1].GetString());
        if (quantity_val > 0.0)
        {
            book.bids[price_val] = quantity_val;
            book.bid_prices.insert(price_val);
        }
        else
        {
            book.bids.erase(price_val);
            book.bid_prices.erase(price_val);
        }
    }
}

// void Binance::parsePrice(std::vector<double> &sell_price, std::vector<double> &buy_price)
// {
//     sell_price.resize(SYMBOLS.size());
//     buy_price.resize(SYMBOLS.size());
//     for (const auto &item : document_.GetArray())
//     {
//         if (item.HasMember("symbol") && item.HasMember("bidPrice") && item.HasMember("askPrice"))
//         {
//             std::unordered_map<std::string, int>::const_iterator look_up = SYMBOL_MAP.find(item["symbol"].GetString());
//             if (look_up != SYMBOL_MAP.end())
//             {
//                 sell_price[look_up->second] = std::stod(item["askPrice"].GetString());
//                 buy_price[look_up->second] = std::stod(item["bidPrice"].GetString());
//             }
//         }
//         else
//             throw std::runtime_error("[Binance] Invalid item encountered");
//     }
// }

} // namespace arbitrage