#pragma once
#include <vector>
#include "Matrix.h"
#include <complex>
#include "json/json.h"

struct UpdateStats{
    double error;
    double y;
    Vec b;
    Json::Value toJson() const;
};

struct FreqzResult{
    FreqzResult(int n): w(n), h(n){}
    std::vector<double> w;
    std::vector<std::complex<double>> h;
    std::vector<std::string> h_toStringVec() const;
};
