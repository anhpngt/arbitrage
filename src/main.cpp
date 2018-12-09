#include <chrono>
#include <thread>


#include "binance.h"
#include "kucoin.h"
#include "exchanges.h"

using namespace std;
using namespace arbitrage;

int main()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    arbitrage::Exchange<Binance, Kucoin> ex;
    ex.run();

    curl_global_cleanup();
    return 0;
}