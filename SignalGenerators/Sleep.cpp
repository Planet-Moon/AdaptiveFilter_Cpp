#include "Sleep.h"

void sleep(const std::chrono::nanoseconds& duration){
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

void sleep(const std::chrono::steady_clock::time_point& start, const std::chrono::nanoseconds& duration){
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
