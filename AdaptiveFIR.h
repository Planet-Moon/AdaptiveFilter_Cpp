#pragma once
#include <complex>
#include <string>
#include "Matrix.h"
#include "json/json.h"

struct FreqzResult{
    FreqzResult(int n): w(n), h(n){}
    std::vector<double> w;
    std::vector<std::complex<double>> h;
    std::vector<std::string> h_toStringVec() const;
};

class AdaptiveFIR{
public:
    AdaptiveFIR(unsigned int n, double p = 0.95);
    Vec filter(Vec b, Vec signal);

    struct UpdateStats{
        double error;
        double y;
        Vec b;
        Json::Value toJson() const;
    };
    UpdateStats update(double x, double d);

    Vec predict(int samples, int delay) const;
    double error() const;

    Vec get_b() const;

    FreqzResult freqz(int samples = 50) const;
    static FreqzResult freqz(const AdaptiveFIR& fir, int samples = 50);
    static FreqzResult freqz(const Vec& b, int samples = 50);

protected:
    const unsigned int n;
    double p;
    Vec x;
    Vec b;
    double e;
    Vec z;
    double y;
    Mat R_inv;
private:
    void update_x(double x);
};
