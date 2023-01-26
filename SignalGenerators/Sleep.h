#pragma once
#include <chrono>

void sleep(const std::chrono::nanoseconds& duration);
void sleep(const std::chrono::steady_clock::time_point& start, const std::chrono::nanoseconds& duration);
