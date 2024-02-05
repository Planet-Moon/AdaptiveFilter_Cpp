#include "gtest/gtest.h"
#include <AdaptiveRLSOrder.h>
#include <iostream>


namespace MV = MatrixVector;

TEST(AdaptiveRLSOrder, Default){
    MV::Vec<5> b = {0.2,0.2,0.2,0.2,0.2};
    auto rls = AdaptiveRLSOrder<5>(0.95);
    auto _b = rls.get_b();
    rls.update(2.0, 0.);
    rls.update(0.0, 0.);
    rls.update(2.0, 0.);
    auto us = rls.update(0.0, 0.);
    auto json = us.toJson();
    EXPECT_TRUE(b.size() == 5);
    MV::Vec<5+1> samples = rls.predict<1>(2);
    return;
}
