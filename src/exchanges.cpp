#include "exchanges.h"

template <class SiteA, class SiteB>
Exchange<SiteA, SiteB>::Exchange() : logged_(false)
{
    // Check if both site have the same values in symbol
    std::cout << "Initializing exchange markets... " << std::endl;

    int size_a = SiteA::SYMBOLS.size();
    int size_b = SiteB::SYMBOLS.size();
    if (size_a != size_b)
        throw std::runtime_error("Sites having different sizes!");
    else
        std::cout << "Number of currencies monitored: " << size_a << std::endl;

    for (auto &item : SiteA::CHECK_MAP)
    {
        auto look_up = SiteB::CHECK_MAP.find(item.first);
        if (look_up == SiteB::CHECK_MAP.end())
            throw std::runtime_error("Cannot find key " + item.first + " from site A in site B");
    }

    std::cout << "Initialization finished" << std::endl;
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
void Exchange<SiteA, SiteB>::getArbitrage()
{
    // Retrieve info
    site_a_.requestPrice();
    site_b_.requestPrice();
    std::vector<double> a_sell, a_buy, b_sell, b_buy;
    site_a_.parsePrice(a_sell, a_buy);
    site_b_.parsePrice(b_sell, b_buy);

    for (int i = 0; i < a_sell.size(); ++i)
    {
        logger_ << std::setprecision(15) << a_sell[i] << "\t" << a_buy[i] << "\t||\t" << b_sell[i] << "\t" << b_buy[i] << std::endl;
    }

    // Calculate
    for (int i = 0; i < a_sell.size(); i++)
    {
        double profit1 = b_buy[i] / a_sell[i] * (1 - site_b_.FEE_TRADE_RATIO) * (1 - site_a_.FEE_TRADE_RATIO);
        double profit2 = a_buy[i] / b_sell[i] * (1 - site_b_.FEE_TRADE_RATIO) * (1 - site_a_.FEE_TRADE_RATIO);
        if (profit1 > 1)
        {
            cout << "Buy " << SiteB::SYMBOLS[i] << " from site B and sell to site A's " << SiteA::SYMBOLS[i] << "\n";
            cout << "Profit ratio: " << profit1 << endl;
        }

        if (profit2 > 1)
        {
            cout << "Buy " << SiteA::SYMBOLS[i] << " from site A and sell to site B's " << SiteB::SYMBOLS[i] << "\n";
            cout << "Profit ratio: " << profit2 << endl;
        }
    }
}

template class Exchange<Binance, Kucoin>;