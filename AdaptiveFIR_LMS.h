#pragma once
#include <complex>
#include <string>
#include "Matrix.h"
#include "AdaptiveFIR.h"
#include "json/json.h"
#include "Fir.h"

class AdaptiveFIR_LMS: public Fir{
public:
    AdaptiveFIR_LMS(unsigned int n, double mu);
    Vec filter(Vec b, Vec signal);

    UpdateStats update(double x, double d);

    Vec predict(int samples, int delay) const;
    double error() const;

protected:
    double mu;
    double e;
    double y;
};
