#pragma once
#include "Matrix.h"

class AdaptiveFIR{
public:
    AdaptiveFIR(unsigned int n, double p = 0.95);
    Vec filter(Vec b, Vec signal);
    void update(double x, double d);
    Vec predict(int samples, int delay) const;
    double error() const;
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
