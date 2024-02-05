#include "gtest/gtest.h"
#include <AdaptiveLMSOrder.h>
#include <iostream>


namespace MV = MatrixVector;

TEST(AdaptiveLMSOrder, Default){
    MV::Vec<5> b = {0.2,0.2,0.2,0.2,0.2};
    auto lms = AdaptiveLMSOrder(b, 0.95);
    auto _b = lms.get_b();
    lms.update(2.0, 0.);
    lms.update(0.0, 0.);
    lms.update(2.0, 0.);
    auto us = lms.update(0.0, 0.);
    auto json = us.toJson();
    EXPECT_TRUE(b.size() == 5);
}
