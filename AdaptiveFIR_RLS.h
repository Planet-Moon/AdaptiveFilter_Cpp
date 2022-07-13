#pragma once
#include <complex>
#include <string>
#include "Matrix.h"
#include "AdaptiveFIR.h"
#include "json/json.h"
#include "Fir.h"


class AdaptiveFIR_RLS: public Fir{
public:
    AdaptiveFIR_RLS(unsigned int n, double p = 0.95);
    Vec filter(Vec b, Vec signal);

    UpdateStats update(double x, double d);

    Vec predict(int samples, int delay) const;
    double error() const;

protected:
    double p;
    double e;
    Vec z;
    double y;
    Mat R_inv;
};
