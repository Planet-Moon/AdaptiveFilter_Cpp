#pragma once
#include <complex>
#include <vector>


struct FreqzResult{
    FreqzResult(int n): w(n), h(n){}
    int size() const;
    std::vector<double> w;
    std::vector<std::complex<double>> h;
    std::vector<double> magnitude() const;
    std::vector<double> phase() const;
    std::vector<std::string> h_toStringVec() const;
};

void write_freqz_to_csv(const std::string &filename, const FreqzResult& freqz);
void write_freqz_to_json(const std::string &filename, const FreqzResult& freqz);
