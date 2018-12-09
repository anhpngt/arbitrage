#include <chrono>
#include <thread>


#include "binance.h"
// #include "kucoin.h"
// #include "exchanges.h"

using namespace std;

int main()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    arbitrage::Binance binance;
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        // static int i = 0;
        // cout << "iter: " << i++ << endl;
    }

    // Exchange<Binance, Kucoin> ex("log.txt");
    // while (true)
    // {
    //     try
    //     {
    //         ex.getArbitrage();
    //     }
    //     catch (...)
    //     {
    //         // simply skip to the next loop for now
    //     }
    //     std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // }

    curl_global_cleanup();
    return 0;
}