#include "gtest/gtest.h"
#include <FirOrder.h>
#include <iostream>


namespace MV = MatrixVector;

TEST(FirOrder, ConstructorWithB){
    MV::Vec<5> b = {0.2, 0.2, 0.2, 0.2, 0.2};
    auto fir = FirOrder(b);
    EXPECT_TRUE(fir.get_n() == b.size());
    auto _b = fir.get_b();
    for(int i = 0; i < b.size(); ++i){
        EXPECT_TRUE(b[i] == _b[i]);
    }

    double value = fir.filter(1.0);
    auto impz_str = fir.impz();
    std::cout << "impz: " << impz_str << std::endl;
}
