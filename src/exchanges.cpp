#include "exchanges.h"

namespace arbitrage
{

template <class SiteA, class SiteB>
Exchange<SiteA, SiteB>::Exchange() : logged_(false)
{
    // Check if both site have the same values in symbol
    std::cout << "\n\n[Exchange] Initializing exchange markets..." << std::endl;

    int size_a = site_a_.getSize();
    int size_b = site_b_.getSize();
    if (size_a != size_b)
        throw std::runtime_error("Sites having different sizes!");
    else
        std::cout << "[Exchange] Number of currencies monitored: " << size_a << std::endl;
    list_size_ = size_a;

    for (auto &item : SiteA::CHECK_MAP)
    {
        auto look_up = SiteB::CHECK_MAP.find(item.first);
        if (look_up == SiteB::CHECK_MAP.end())
            throw std::runtime_error("Cannot find key " + item.first + " from " + site_a_.name() + " in " + site_b_.name());
    }

    std::cout << "[Exchange] Initialization finished" << std::endl;
};

template <class SiteA, class SiteB>
Exchange<SiteA, SiteB>::Exchange(const std::string &log_file)
    : log_file_(log_file), logged_(true)
{
    Exchange();

    std::cout << "Writing logs to " << log_file_ << std::endl;
    logger_.open(log_file_);
}

template <class SiteA, class SiteB>
void Exchange<SiteA, SiteB>::run()
{
    std::cout << "[Exchange] Seaching for arbitrage opporunity... " << std::endl;
    while (true)
    {
        if (!(site_a_.getStatus() || site_b_.getStatus()))
        {
            static int count = 0;
            if (++count == 10)
                throw std::runtime_error("ERROR: Unable to start");
            continue;
        }

        // Calculate arbitrage
        std::vector<OrderBook> book_a = site_a_.getBook();
        std::vector<OrderBook> book_b = site_b_.getBook();

        // Iterate through each coin type
        PROFILER_BEGIN(arbitrage);
        for (size_t idx = 0; idx < list_size_; ++idx)
        {
            // Buy from A and sell to B?
            searchArbitrage(book_a[idx], book_b[idx], SiteA::SYMBOLS[idx], "buy at " + site_a_.name() + ", sell at " + site_b_.name());

            // Buy from B and sell to A?
            searchArbitrage(book_b[idx], book_a[idx], SiteA::SYMBOLS[idx], "buy at " + site_b_.name() + ", sell at " + site_a_.name());
        }
        PROFILER_END(arbitrage);
    }
}

template <class SiteA, class SiteB>
void Exchange<SiteA, SiteB>::searchArbitrage(OrderBook &ask_market, OrderBook &bid_market,
                                             const std::string &coin_market_name, const std::string &action,
                                             double ask_fee, double bid_fee)
{
    auto ask_price_iter = ask_market.ask_prices.begin();  // lowest ask price
    auto bid_price_iter = bid_market.bid_prices.rbegin(); // highest bid price

    auto ask_price_end = ask_market.ask_prices.end();
    auto bid_price_end = bid_market.bid_prices.rend();

    auto &ask_book = ask_market.asks;
    auto &bid_book = bid_market.bids;

    // Search for arbitrage opportunity from the lowest ask price and highest bid price
    while (ask_price_iter != ask_price_end && bid_price_iter != bid_price_end)
    {
        double profit_ratio = std::stod(*bid_price_iter) * (1 - bid_fee) - std::stod(*ask_price_iter) * (1 + ask_fee);
        if (profit_ratio > 0)
        {
            // Profit opporunity
            // We purchased all quanity available
            double &ask_quantity = ask_book[*ask_price_iter];
            double &bid_quantity = bid_book[*bid_price_iter];
            if (ask_quantity > bid_quantity)
            {
                // There are more quantity available to buy than to sell
                std::cout << "[Arbitrage] " + action + ". Coin market: " << coin_market_name
                          << ". Ask price : " << *ask_price_iter
                          << ". Bid price : " << *bid_price_iter
                          << ". Purchase quantity: " << bid_quantity
                          << ". Profit: " << profit_ratio * bid_quantity << std::endl;

                // Update quantity for further next price level compute
                ask_quantity -= bid_quantity;
                ++ask_price_iter;
            }
            else
            {
                // There are more quantity available to buy than to sell
                std::cout << "[Arbitrage] " + action + ". Coin market: " << coin_market_name
                          << ". Ask price : " << *ask_price_iter
                          << ". Bid price : " << *bid_price_iter
                          << ". Purchase quantity: " << ask_quantity
                          << ". Profit: " << profit_ratio * ask_quantity << std::endl;

                // Update quantity for further next price level compute
                bid_quantity -= ask_quantity;
                ++bid_price_iter;
            }
        }
        else
        {
            break;
        }
    }
}

// template <class SiteA, class SiteB>
// void Exchange<SiteA, SiteB>::getArbitrage()
// {
//     // Retrieve info
//     PROFILER_BEGIN(Requesting);
//     // if (!site_a_.requestPrice())
//     //     throw std::runtime_error("Cannot retrieve data from " + site_a_.name());
//     // if (!site_b_.requestPrice())
//     //     throw std::runtime_error("Cannot retrieve data from " + site_b_.name());
//     std::future<bool> a = std::async(std::launch::async, &SiteA::requestPrice, &site_a_);
//     std::future<bool> b = std::async(std::launch::async, &SiteB::requestPrice, &site_b_);
//     if (!(a.get() && b.get()))
//         throw std::runtime_error("Cannot retrieve data from site");

//     PROFILER_END(Requesting);

//     PROFILER_BEGIN(Parsing);
//     std::vector<double> a_sell, a_buy, b_sell, b_buy;
//     site_a_.parsePrice(a_sell, a_buy);
//     site_b_.parsePrice(b_sell, b_buy);
//     PROFILER_END(Parsing);

//     PROFILER_BEGIN(Logging);
//     for (int i = 0; i < a_sell.size(); ++i)
//     {
//         logger_ << std::fixed << std::setprecision(6)
//                 << SiteA::SYMBOLS[i] << " : "
//                 << a_sell[i] << "\t" << a_buy[i] << "\t||\t" << b_sell[i] << "\t" << b_buy[i] << std::endl;
//     }
//     logger_ << "\n\n"
//             << std::endl;
//     PROFILER_END(Logging);

//     // Calculate
//     PROFILER_BEGIN(Computation);
//     for (int i = 0; i < a_sell.size(); i++)
//     {
//         double profit1 = b_buy[i] * (1 - site_b_.FEE_TRADE_RATIO) - a_sell[i] * (1 + site_a_.FEE_TRADE_RATIO);
//         double profit2 = a_buy[i] * (1 - site_a_.FEE_TRADE_RATIO) - b_sell[i] * (1 + site_b_.FEE_TRADE_RATIO);

//         if (profit1 > 0)
//         {
//             cout << "\033[32mBuy 1 " << SiteA::SYMBOLS[i] << " coin at " << site_a_.name()
//                  << ", sell 1 " << SiteB::SYMBOLS[i] << " coin at " << site_b_.name() << "\n"
//                  << "Profit ratio: " << profit1 << "\033[0m" << endl;
//             cout << "\tb_buy = " << b_buy[i] << "\ta_sell = " << a_sell[i] << endl;
//         }

//         if (profit2 > 0)
//         {
//             cout << "\033[32mBuy 1 " << SiteB::SYMBOLS[i] << " coin at " << site_b_.name()
//                  << ", sell 1 " << SiteA::SYMBOLS[i] << " coin at " << site_a_.name() << "\n"
//                  << "Profit ratio: " << profit2 << "\033[0m" << endl;
//             cout << "\ta_buy = " << a_buy[i] << "\tb_sell = " << b_sell[i] << endl;
//         }
//     }
//     PROFILER_END(Computation);
// }

template class Exchange<Binance, Kucoin>;
template class Exchange<Kucoin, Binance>;

} // namespace arbitrage