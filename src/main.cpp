#include "binance.h"
#include "kucoin.h"

#include <chrono>
#include <thread>

int main()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    Binance b;
    Kucoin k;

    double binance_sell, binance_buy;
    double kucoin_sell, kucoin_buy;
    while(true)
    {
        if(b.request())
            b.printResult();
            // b.parse(binance_buy, binance_sell);
        if(k.request())
            k.printResult();
            // k.parse(kucoin_buy, kucoin_sell);

        // double profit1 = kucoin_buy / binance_sell * (1 - k.FEE_TRADE_RATIO) * (1 - b.FEE_TRADE_RATIO);
        // double profit2 = binance_buy / kucoin_sell * (1 - k.FEE_TRADE_RATIO) * (1 - b.FEE_TRADE_RATIO);
        // // if( profit1 > 1 )
        // {
        //     cout << "Buy from Kucoin and sell to Binance\n";
        //     cout << "Profit ratio: " << profit1 << endl;
        // }

        // // if( profit2 > 1 )
        // {
        //     cout << "Buy from Binance and sell to Kucoin\n";
        //     cout << "Profit ratio: " << profit2 << endl;
        // }

        std::this_thread::sleep_for (std::chrono::milliseconds(100));
    }
    
    curl_global_cleanup();
    return 0;
}