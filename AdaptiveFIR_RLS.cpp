#include "AdaptiveFIR_RLS.h"
#include <Matrix.h>
#include <cassert>
#include <cmath>

const double PI = std::acos(-1);
using namespace std::complex_literals;

AdaptiveFIR_RLS::AdaptiveFIR_RLS(unsigned int n, double p):
    Fir(n), p(p)
{
    R_inv = 1e9 * Matrix::identity(n);
    e = 0.;
    z = Vector::zeros(n);
    y = 0.;
}

Vec AdaptiveFIR_RLS::filter(Vec b, Vec signal)
{
    const int signal_dim = signal.size();
    Vec result(signal_dim);
    #pragma omp parallel for
    for(int i = n-1; i < signal_dim; ++i){
        double fir_sum = 0;
        #pragma omp parallel for
        for(int n_i = 0; n_i < n; ++n_i){
            fir_sum += signal[i-n_i] * b[n_i];
        }
        result[i] = fir_sum;
    }
    return result;
}

UpdateStats AdaptiveFIR_RLS::update(double _x, double d)
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
    UpdateStats us{};
    us.b = b;
    us.error = abs(e);
    us.y = y;
    return us;
}

Vec AdaptiveFIR_RLS::predict(int samples, int delay) const
{
    Vec result(samples + n);
    #pragma omp parallel for
    for(int i = 0; i < x.size(); i++){
        result[i] = x[i];
    }
    #pragma omp parallel for
    for(int i = x.size(); i < result.size(); i++){
        const Vec local_x(i-(i-n));
        auto z = Matrix::fromVector(local_x) * b;
        result[i] = z[0];
    }
    return result;
}

double AdaptiveFIR_RLS::error() const
{
    return e;
}
