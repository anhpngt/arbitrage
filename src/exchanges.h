#ifndef EXCHANGES_H_
#define EXCHANGES_H_

#include <string>
#include <iostream>
#include <fstream>

#include "binance.h"
#include "kucoin.h"

namespace arbitrage
{

template <class SiteA, class SiteB>
class Exchange
{
  public:
    /** Default constructor */
    Exchange();

    /** Constructor with log writing */
    Exchange(const std::string &log_file);

    /** Compute arbitrage. This is a blocking function */
    // void getArbitrage();
    void run();

  private:
    void searchArbitrage(OrderBook &ask_market, OrderBook &bid_market,
                         const std::string &coin_market_name, const std::string &action,
                         double ask_fee = 0.001, double bid_fee = 0.001);

    SiteA site_a_;
    SiteB site_b_;
    size_t list_size_;
    std::string log_file_;
    bool logged_;
    std::ofstream logger_;
};

} // namespace arbitrage

#endif