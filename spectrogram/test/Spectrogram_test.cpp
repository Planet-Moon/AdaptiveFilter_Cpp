#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <thread>
#include "gtest/gtest.h"
#include <Spectrogram.h>

TEST(Spectrogram, Build){
    EXPECT_TRUE(true);
    EXPECT_EQ(2, 1+1);
}

TEST(Spectrogram, Constructor1){
    Spectrogram spectrogram;
}

TEST(Spectrogram, Constructor2){
    Spectrogram spectrogram(1024);
}

void sleep(std::chrono::nanoseconds duration){
    const auto start = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point now;
    std::chrono::nanoseconds elapsed;
    while (true)
    {
        now = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - start);
        if(elapsed.count() > duration.count()){
            break;
        }
    }
}

std::vector<std::complex<double>> complexVec(std::complex<double> input){
    return {input};
}

TEST(Spectrogram, ComplexAbsolute){
    EXPECT_EQ(
        Spectrogram::abs(complexVec({1.0, 1.0})).front(),
        std::pow(std::pow(1,2)+std::pow(1,2), 0.5));

    EXPECT_EQ(
        Spectrogram::abs(complexVec({1.0, 0})).front(),
        1);

    EXPECT_EQ(
        Spectrogram::abs(complexVec({0, 1.0})).front(),
        1);

    EXPECT_EQ(
        Spectrogram::abs(complexVec({0, 0})).front(),
        0);

    EXPECT_EQ(
        Spectrogram::abs(complexVec({10.0, 10.0})).front(),
        std::pow(std::pow(10,2)+std::pow(10,2), 0.5));

    EXPECT_EQ(
        Spectrogram::abs(complexVec({10.0, 0})).front(),
        10);

    EXPECT_EQ(
        Spectrogram::abs(complexVec({0, 10.0})).front(),
        10);

    EXPECT_EQ(
        Spectrogram::abs(complexVec({0, 0})).front(),
        0);
}

TEST(Spectrogram, ComplexArgument) {
    EXPECT_EQ(Spectrogram::arg(complexVec({1.0, 1.0})).front(), M_PI*0.25);
    EXPECT_EQ(Spectrogram::arg(complexVec({1.0, 0})).front(), M_PI*0);
    EXPECT_EQ(Spectrogram::arg(complexVec({0, 1.0})).front(), M_PI*0.5);
    EXPECT_EQ(Spectrogram::arg(complexVec({0, 0})).front(), M_PI*0);

    EXPECT_EQ(Spectrogram::arg(complexVec({10.0, 10.0})).front(), M_PI*0.25);
    EXPECT_EQ(Spectrogram::arg(complexVec({10.0, 0})).front(), M_PI*0);
    EXPECT_EQ(Spectrogram::arg(complexVec({0, 10.0})).front(), M_PI*0.5);
    EXPECT_EQ(Spectrogram::arg(complexVec({0, 0})).front(), M_PI*0);
}

TEST(Spectrogram, Decibel) {
    EXPECT_LE(Spectrogram::toDecibel({2}).front(), 6.0207);
    EXPECT_GT(Spectrogram::toDecibel({2}).front(), 6.0205);

    EXPECT_LE(Spectrogram::fromDecibel({6.0205}).front(), 2);
    EXPECT_GT(Spectrogram::fromDecibel({6.0207}).front(), 2);

    const double root_2 = std::pow(2, 0.5);
    EXPECT_LE(Spectrogram::toDecibel({root_2}).front(), 3.0104);
    EXPECT_GT(Spectrogram::toDecibel({root_2}).front(), 3.0102);

    EXPECT_LE(Spectrogram::fromDecibel({3.0102}).front(), root_2);
    EXPECT_GT(Spectrogram::fromDecibel({3.0104}).front(), root_2);
}

TEST(Spectrogram, addSample){
    Spectrogram s(16*2);

    for(int i=0; i<512; i++){
        s.addSample(i);
        sleep(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::microseconds(1000)));
    }
    const auto deviation = abs((s.sampleFrequency()/1000)-1);
    EXPECT_LE(deviation, 0.13);
}
