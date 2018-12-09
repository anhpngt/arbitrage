#include "kucoin.h"

namespace arbitrage
{

const std::vector<std::string> Kucoin::SYMBOLS = {
    "BTC-USDT",
    "ETH-USDT",
    "LTC-USDT",
    "NEO-USDT"};

const std::unordered_map<std::string, int> Kucoin::SYMBOL_MAP = {
    {"BTC-USDT", 0},
    {"ETH-USDT", 1},
    {"LTC-USDT", 2},
    {"NEO-USDT", 3}};

const std::unordered_map<std::string, std::string> Kucoin::CHECK_MAP = {
    {"BTC", "BTC-USDT"},
    {"ETH", "ETH-USDT"},
    {"LTC", "LTC-USDT"},
    {"NEO", "NEO-USDT"}};

const std::vector<std::string> Kucoin::TOPIC_NAMES = {
    "/trade/BTC-USDT_TRADE",
    "/trade/ETH-USDT_TRADE",
    "/trade/LTC-USDT_TRADE",
    "/trade/NEO-USDT_TRADE"};

const std::string Kucoin::RESTAPI_BASE_ENDPOINT = "https://api.kucoin.com";
const std::string Kucoin::RESTAPI_ORDER_BOOK_ENDPOINT = "/v1/open/orders";
const std::string Kucoin::WS_BASE_ENDPOINT = "wss://push1.kucoin.com/endpoint";
const int Kucoin::ORDER_BOOK_LIMIT = 100;

const std::string Kucoin::WEBSOCKET_REQUEST_ENDPOINT = "https://kitchen.kucoin.com/v1/bullet/usercenter/loginUser?protocol=websocket&encrypt=true";
const int Kucoin::PING_INTERVAL = 10000;

Kucoin::Kucoin() : API("Kucoin"), is_initialized_(false), list_size_(SYMBOLS.size()), is_ws_connected_(false)
{
    // Acquire Kucoin websocket server through REST
    while (true)
    {
        requestRestApi(WEBSOCKET_REQUEST_ENDPOINT);
        if (document_.HasMember("success"))
            if (document_["success"].GetBool())
                break;

        std::cout << "[Kucoin] Failed to acquired websocket server. Retrying... " << std::endl;
    }

    bullet_token_ = document_["data"]["bulletToken"].GetString();
    ws_base_endpoint_ = document_["data"]["instanceServers"][0]["endpoint"].GetString();

    // Connect to websocket server
    std::string websocket_endpoint = ws_base_endpoint_ + "?bulletToken=" + bullet_token_ + "&format=json&resource=api";
    std::cout << "[Kucoin] Websocket endpoint: " << websocket_endpoint << std::endl;
    std::cout << "[Kucoin] Connecting to websocket" << std::endl;
    while (true)
    {
        try
        {
            client_.connect(U(websocket_endpoint)).wait();
            break;
        }
        catch (web::websockets::client::websocket_exception &e)
        {
            cout << e.what() << endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    while (!is_ws_connected_)
    {
        std::this_thread::yield(); // Block until connected to websocket
    }

    // Subscribe to required topics
    for (size_t i = 0; i < list_size_; ++i)
    {
        std::string msg_str = "{\"id\":123, \"type\":\"subscribe\", \"topic\":\"" + TOPIC_NAMES[i] + "\", \"req\":0}";
        cout << "[Kucoin] Subscribing to " << msg_str << endl;
        web::web_sockets::client::websocket_outgoing_message msg;
        msg.set_utf8_message(msg_str);
        client_.send(msg).wait();
    }
    std::cout << "[Kucoin] Finished subscribing to topics" << std::endl;

    // Query all orderbook data for the first time
    intializeOrderBook();

    // Set up periodic pinging
    msg_ping_.set_utf8_message("{\"id\":123, \"type\":\"ping\"}");
    pinger_thread_ = std::thread(&Kucoin::pingServer, this);
    pinger_thread_.detach();

    std::cout << "[Kucoin] Initialization finished." << std::endl;
}

void Kucoin::intializeOrderBook()
{
    std::lock_guard<std::mutex> lck(init_mutex_);
    if (is_initialized_)
        return;

    std::cout << "[Kucoin] Initializing order book" << std::endl;

    order_book_.clear();
    order_book_.resize(list_size_);
    for (size_t i = 0; i < list_size_; ++i)
    {
        // Request until success
        std::string restapi_url = RESTAPI_BASE_ENDPOINT + RESTAPI_ORDER_BOOK_ENDPOINT + "?symbol=" + SYMBOLS[i] + "&limit=" + std::to_string(ORDER_BOOK_LIMIT);
        while (!requestRestApi(restapi_url))
        {
            std::this_thread::yield();
        }

        if (!(document_.HasMember("success") && document_.HasMember("data")))
            throw std::runtime_error("[Kucoin] Orderbook init error: missing queries in JSON data");
        else if (!document_["success"].GetBool())
            throw std::runtime_error("[Kucoin] Requesting orderbook unsuccessful (success code = false)");

        updateBook(order_book_[i], document_["data"]["SELL"], document_["data"]["BUY"]);
    }

    is_initialized_ = true;
}

void Kucoin::updateOrderBookCallback(const web::web_sockets::client::websocket_incoming_message &msg)
{
    std::string msg_str = msg.extract_string().get();
    if (!is_ws_connected_)
    {
        // the acknowledgement message for connection
        rapidjson::Document parser;
        if (!parser.Parse(msg_str.c_str()).HasParseError())
        {
            if (parser.HasMember("id") && parser.HasMember("type"))
            {
                std::string msg_type = parser["type"].GetString();
                if (msg_type == "ack" || msg_type == "pong")
                {
                    // Successfully init
                    std::cout << "[Kucoin] Websocket connected" << std::endl;
                    is_ws_connected_ = true;
                    return;
                }
            }
        }

        throw std::runtime_error("[Kucoin] Unable to connect to websocket?");
    }

    if (!is_initialized_)
    {
        cout << "[Kucoin] Received callback but not init yet" << endl;
        intializeOrderBook();
        return;
    }

    rapidjson::Document parser;
    parser.Parse<rapidjson::ParseFlag::kParseNumbersAsStringsFlag>(msg_str.c_str());

    // Check if correct key values
    if (!(parser.HasMember("topic") && parser.HasMember("data")))
        return;
    else if (!(parser["data"].HasMember("price") && parser["data"].HasMember("count")))
        return;

    // Update order book
    auto lookup = std::find(TOPIC_NAMES.begin(), TOPIC_NAMES.end(), parser["topic"].GetString());
    if (lookup == TOPIC_NAMES.end())
        throw std::runtime_error(std::string("Cannot find matched stream name of: ") + parser["topic"].GetString());

    size_t list_idx = lookup - TOPIC_NAMES.begin();
    std::string action = parser["data"]["action"].GetString();
    std::string type = parser["data"]["type"].GetString();
    std::string price_val = parser["data"]["price"].GetString();
    double amount_val = std::stod(parser["data"]["count"].GetString());

    {
        std::lock_guard<std::mutex> ob_lck(order_book_mutex_);
        OrderBook &book = order_book_[list_idx];
        if (action == "ADD")
        {
            if (type == "SELL")
            {
                book.asks[price_val] = amount_val;
                book.ask_prices.insert(price_val);
            }
            else if (type == "BUY")
            {
                book.bids[price_val] = amount_val;
                book.bid_prices.insert(price_val);
            }
        }
        else if (action == "CANCEL")
        {
            if (type == "SELL")
            {
                book.asks.erase(price_val);
                book.ask_prices.erase(price_val);
            }
            else if (type == "BUY")
            {
                book.bids.erase(price_val);
                book.bid_prices.erase(price_val);
            }
        }
    }

    // printOrderBook(0);
}

void Kucoin::updateBook(OrderBook &book, const rapidjson::Value &asks, const rapidjson::Value &bids)
{
    assert(asks.IsArray());
    assert(bids.IsArray());
    std::lock_guard<std::mutex> ob_lck(order_book_mutex_);

    // Each array format is [price, amount, volume]
    for (const auto &data : asks.GetArray())
    {
        std::string price_val = data[0].GetString();
        double quantity_val = std::stod(data[1].GetString());
        book.asks[price_val] = quantity_val;
        book.ask_prices.insert(price_val);
    }
    for (const auto &data : bids.GetArray())
    {
        std::string price_val = data[0].GetString();
        double quantity_val = std::stod(data[1].GetString());
        book.bids[price_val] = quantity_val;
        book.bid_prices.insert(price_val);
    }
}

void Kucoin::pingServer()
{
    while (true)
    {
        client_.send(msg_ping_).wait();
        std::this_thread::sleep_for(std::chrono::milliseconds(PING_INTERVAL));
    }
}

} // namespace arbitrage