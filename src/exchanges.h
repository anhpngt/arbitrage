#ifndef EXCHANGES_H_
#define EXCHANGES_H_

#include <string>
#include <iostream>
#include <fstream>

#include "api.h"
#include "binance.h"
#include "kucoin.h"

template <class SiteA, class SiteB>
class Exchange
{
  public:
    Exchange();

    Exchange(const std::string &log_file);

    void getArbitrage();

  private:
    SiteA site_a_;
    SiteB site_b_;
    std::string log_file_;
    bool logged_;
    std::ofstream logger_;
};

#endif