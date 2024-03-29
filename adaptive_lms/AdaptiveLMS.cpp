#include "AdaptiveLMS.h"

const double PI = std::acos(-1);
using namespace std::complex_literals;

AdaptiveLMS::AdaptiveLMS(unsigned int n, double mu):
    Fir(n), mu(mu)
{
    e = 0.;
    y = 0.;
}

Vec AdaptiveLMS::filter(Vec b, Vec signal)
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

UpdateStats AdaptiveLMS::update(double _x, double d)
{
    update_x(_x);
    y = 0;
    for(int i = 0; i < n; i++){
        y += b[i] * x[i];
    }
    e = d - y;

    for(int i = 0; i < n; ++i){
        b[i] = b[i] + mu * e * x[i];
    }

    UpdateStats us{};
    us.b = b;
    us.error = e;
    us.y = y;
    return us;
}

Vec AdaptiveLMS::predict(int samples, int delay) const
{
    return{};
}

double AdaptiveLMS::error() const
{
    return e;
}
