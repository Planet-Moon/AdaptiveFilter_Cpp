#include "gtest/gtest.h"
#include <InputThread.h>
#include <Sleep.h>

TEST(InputThread, Constructor){
    InputThread inputTh;
}

TEST(InputThread, Generate){
    InputThread inputTh;
    uint16_t n_samples = 100;
    std::vector<double> samples(n_samples);
    std::vector<double> micro_seconds(n_samples);
    const std::chrono::steady_clock::time_point run_start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point start;
    for(int i = 0; i < n_samples; ++i){
        start = std::chrono::steady_clock::now();
        micro_seconds[i] = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(std::chrono::steady_clock::now() - run_start).count();
        samples[i] = inputTh.next();
        sleep(start, std::chrono::microseconds(20));
    }
    EXPECT_EQ(true, true);
}

TEST(InputThread, SinusFrequencyChange){
    InputThread inputTh;
    uint16_t n_samples = 100;
    std::vector<double> samples(n_samples);
    std::vector<double> micro_seconds(n_samples);
    const std::chrono::steady_clock::time_point run_start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point start;
    for(int i = 0; i < n_samples; ++i){
        start = std::chrono::steady_clock::now();
        micro_seconds[i] = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(std::chrono::steady_clock::now() - run_start).count();
        samples[i] = inputTh.next();
        if(i == 40){
            inputTh.getSinusGenerator()->setFrequency(1000);
        }
        sleep(start, std::chrono::microseconds(20));
    }
    EXPECT_EQ(inputTh.getSinusGenerator()->getFrequency(), 1000);
}
