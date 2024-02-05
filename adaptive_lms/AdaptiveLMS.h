#pragma once
#include <Matrix.h>
#include <Fir.h>

class AdaptiveLMS: public Fir{
public:
    AdaptiveLMS(unsigned int n, double mu);
    Vec filter(Vec b, Vec signal);

    UpdateStats update(double x, double d);

    Vec predict(int samples, int delay) const;
    double error() const;

protected:
    double mu;
    double e;
    double y;
};
