#include "exchanges.h"

template <class SiteA, class SiteB>
Exchange<SiteA, SiteB>::Exchange() : logged_(false)
{
    // Check if both site have the same values in symbol
    std::cout << "Initializing exchange markets..." << std::endl;

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
            throw std::runtime_error("Cannot find key " + item.first + " from " + site_a_.name() + " in " + site_b_.name());
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
    PROFILER_BEGIN(Requesting);
    if (!site_a_.requestPrice())
        throw std::runtime_error("Cannot retrieve data from " + site_a_.name());
    if (!site_b_.requestPrice())
        throw std::runtime_error("Cannot retrieve data from " + site_b_.name());
    PROFILER_END(Requesting);

    PROFILER_BEGIN(Parsing);
    std::vector<double> a_sell, a_buy, b_sell, b_buy;
    site_a_.parsePrice(a_sell, a_buy);
    site_b_.parsePrice(b_sell, b_buy);
    PROFILER_END(Parsing);

    PROFILER_BEGIN(Logging);
    for (int i = 0; i < a_sell.size(); ++i)
    {
        logger_ << std::fixed << std::setprecision(6) 
                << SiteA::SYMBOLS[i] << " : "
                << a_sell[i] << "\t" << a_buy[i] << "\t||\t" << b_sell[i] << "\t" << b_buy[i] << std::endl;
    }
    logger_ << "\n\n"
            << std::endl;
    PROFILER_END(Logging);

    // Calculate
    PROFILER_BEGIN(Computation);
    for (int i = 0; i < a_sell.size(); i++)
    {
        double profit1 = b_buy[i] / a_sell[i] * (1 - site_b_.FEE_TRADE_RATIO) * (1 - site_a_.FEE_TRADE_RATIO);
        double profit2 = a_buy[i] / b_sell[i] * (1 - site_b_.FEE_TRADE_RATIO) * (1 - site_a_.FEE_TRADE_RATIO);
        if (profit1 > 1 && profit1 < 999)
        {
            cout << "\033[32mBuy " << SiteB::SYMBOLS[i] << " / " << site_b_.name()
                 << " and sell " << site_a_.name() << " / " << SiteA::SYMBOLS[i] << "\n"
                 << "Profit ratio: " << profit1 << "\033[0m" << endl;
        }

        if (profit2 > 1 && profit2 < 999)
        {
            cout << "\033[32mBuy " << SiteA::SYMBOLS[i] << " / " << site_a_.name()
                 << " and sell " << site_b_.name() << " / " << SiteB::SYMBOLS[i] << "\n"
                 << "Profit ratio: " << profit2 << "\033[0m" << endl;
        }
    }
    PROFILER_END(Computation);
}

template class Exchange<Binance, Kucoin>;