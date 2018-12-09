#include "api.h"

namespace arbitrage
{

API::API(const std::string &exchange_name) : exchange_name_(exchange_name), headers_(nullptr)
{
    // Setup connections
    curl_ = curl_easy_init();
    if (curl_)
    {
        headers_ = curl_slist_append(headers_, "content-Type: application/json");
        headers_ = curl_slist_append(headers_, "charsets: utf-8");

        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers_);
        curl_easy_setopt(curl_, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
        // curl_easy_setopt(curl_, CURLOPT_VERBOSE, 1L);            // verboseness
        // curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 10);            // time-out
        // curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);     // follows redirect
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, arbitrage::API::writeData);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &http_data_);
    }

    // Setup connection for websocket
    client_.set_message_handler(std::bind(&API::updateOrderBookCallback, this, std::placeholders::_1));
}

void API::release()
{
    curl_slist_free_all(headers_);
    curl_easy_cleanup(curl_);

    client_.close().wait();
}

API::~API()
{
    release();
}

bool API::requestRestApi(const std::string &url)
{
    http_data_.clear();
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    res_ = curl_easy_perform(curl_);
    if (res_ == CURLE_OK)
        curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_code_);

    if (http_code_ == 200)
    {
        if (!document_.Parse<rapidjson::ParseFlag::kParseNumbersAsStringsFlag>(http_data_.c_str()).HasParseError())
        {
            return true;
        }
        else
        {
            cout << "ERROR: Could not parse HTTP data as JSON" << endl;
            cout << "HTTP data was:\n"
                 << http_data_ << endl;
            return false;
        }
    }
    else
    {
        cout << "ERROR: Could not get data from URL, HTTP code: " << http_code_ << endl;
        return false;
    }
}

void API::printOrderBook(const size_t coin_idx)
{
    std::lock_guard<std::mutex> ob_lck(order_book_mutex_);
    OrderBook &book = order_book_[(coin_idx >= order_book_.size() ? order_book_.size() - 1 : coin_idx)];
    cout << "Asks:\n";
    cout << std::setw(10) << "Price" << std::setw(15) << "Quantity\n";
    int count = 0;
    for (auto it = book.ask_prices.begin(), it_end = book.ask_prices.end(); it != it_end; ++it)
    {
        cout << std::setw(10) << *it << std::setw(15) << std::setprecision(10) << book.asks[*it] << "\n";
        if (++count > 5)
            break;
    }

    cout << "\nBids\n";
    cout << std::setw(10) << "Price" << std::setw(15) << "Quantity\n";
    count = 0;
    for (auto it = book.bid_prices.rbegin(), it_end = book.bid_prices.rend(); it != it_end; ++it)
    {
        cout << std::setw(10) << *it << std::setw(15) << std::setprecision(10) << book.bids[*it] << "\n";
        if (++count > 5)
            break;
    }

    cout << endl;
}

std::size_t API::writeData(const char *in, std::size_t size, std::size_t num, std::string *out)
{
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}

} // namespace arbitrage