#pragma once
#include <string>
#include <Matrix.h>
#include <FilterReturnTypes.h>

class Fir{
public:
    Fir(const Vec& b);

    double filter(const double& x);

    std::string impz() const;

    Vec get_b() const;
    unsigned int get_n() const;

    FreqzResult freqz(int samples = 50) const;
    static FreqzResult freqz(const Fir& fir, int samples = 50);
    static FreqzResult freqz(const Vec& b, int samples = 50);

protected:
    Fir(unsigned int& n);
    Vec b;
    Vec x;
    const unsigned int n;
    virtual void update_x(const double& x) final;
};
