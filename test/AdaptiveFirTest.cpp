#include "gtest/gtest.h"
#include <AdaptiveRLS.h>
#include <iostream>

TEST(AdaptiveFir, Build){
    EXPECT_TRUE(true);
    EXPECT_EQ(2, 1+1);
}

TEST(AdaptiveFir, Constructor){
    AdaptiveRLS fir(5);
}

TEST(AdaptiveFir, Filter){
    AdaptiveRLS fir(5);
    Vec b = {1,1,1,1,1};
    Vec signal = {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,0.10,0.11};
    Vec filtered_signal = fir.filter(b, signal);
}

TEST(AdaptiveFir, Update){
    AdaptiveRLS fir(5);
    for(int i = 0; i < 1e3; i++){
        fir.update(0.0, 0.3);
        fir.update(-0.1, 0.3);
        fir.update(-0.2, 0.3);
        fir.update(-0.3, 0.3);
        fir.update(-0.3, 0.3);
        fir.update(-0.2, 0.3);
        fir.update(-0.1, 0.3);
        fir.update(0.0, 0.3);
        fir.update(0.1, 0.3);
        fir.update(0.2, 0.3);
        fir.update(0.3, 0.3);
        fir.update(0.3, 0.3);
        fir.update(0.2, 0.3);
        fir.update(0.1, 0.3);
        // std::cout << "fir.e: " << fir.error() << std::endl;
    }
    auto predict = fir.predict(2,0);
}
