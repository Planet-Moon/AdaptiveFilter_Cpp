#include "AdaptiveFIR.h"
#include <Matrix.h>
#include <cassert>

AdaptiveFIR::AdaptiveFIR(unsigned int n, double p):
    n(n), p(p)
{
    R_inv = 1e9 * Matrix::identity(n);
    x = Vector::zeros(n);
    b = Vector::zeros(n);
    e = 0.;
    z = Vector::zeros(n);
    y = 0.;
}

Vec AdaptiveFIR::filter(Vec b, Vec signal)
{
    const int signal_dim = signal.size();
    Vec result(signal_dim);
    for(int i = n-1; i < signal_dim; ++i){
        double fir_sum = 0;
        for(int n_i = 0; n_i < n; ++n_i){
            fir_sum += signal[i-n_i] * b[n_i];
        }
        result[i] = fir_sum;
    }
    return result;
}

void AdaptiveFIR::update_x(double _x)
{
    if(x.size() > 1)
        std::rotate(x.rbegin(), x.rbegin()+1, x.rend());
    x[0] = _x;
}

void AdaptiveFIR::update(double _x, double d)
{
    update_x(_x);
    Mat m_x = Matrix::fromVector(x);
    Vec y_v = m_x * b;
    double y = y_v[0];
    e = d - y;
    Mat x_m = Matrix::fromVector(x);
    Vec z;
    {
        Vec numerator = R_inv * x;
        Vec denominator_p = x_m * (R_inv * x);
        double denominator = p + denominator_p[0];
        assert(denominator != 0);
        z = numerator * (1/denominator);
    }
    b = b + e*z;
    auto z1 = x_m * R_inv;
    auto z_m = Matrix::fromVector(z);
    auto z_m_t = Matrix::transpose(z_m);
    auto z2 = z_m_t * z1;
    auto z3 = R_inv - z2;
    R_inv = 1/p * z3;
}

Vec AdaptiveFIR::predict(int samples, int delay) const
{
    Vec result(samples + n);
    for(int i = 0; i < x.size(); i++){
        result[i] = x[i];
    }
    for(int i = x.size(); i < result.size(); i++){
        const Vec local_x(i-(i-n));
        auto z = Matrix::fromVector(local_x) * b;
        result[i] = z[0];
    }
    return result;
}

double AdaptiveFIR::error() const
{
    return e;
}
