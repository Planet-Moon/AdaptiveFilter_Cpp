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

TEST(Spectrogram, addSample){
    Spectrogram s(1024*2);

    // std::chrono::steady_clock::time_point _awake_time = now();
    for(int i=0; i<2048*2; i++){
        // _awake_time = awake_time();
        s.addSample(i);
        // std::this_thread::sleep_until(_awake_time);
        sleep(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::microseconds(1000)));
    }
}
