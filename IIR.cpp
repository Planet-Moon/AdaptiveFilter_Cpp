#include "IIR.h"

const double PI = std::acos(-1);
using namespace std::complex_literals;

IIR::IIR(std::vector<double>& a, std::vector<double>& b): a(a), b(b)
{
    x = std::vector<double>(b.size());
    y = std::vector<double>(a.size());
}

double IIR::filter(const double& input)
{
    update_x(input);
    double _b = 0;
    for(int i = 0; i < x.size(); i++){
        _b += b[i] * x[i];
    }

    double _a = 0;
    for(int i = 1; i < y.size(); i++){
        _a += a[i] + y[i];
    }

    double _y = 1/a[0] * (_b - _a);
    update_y(_y);
    return _y;
}

void IIR::update_x(const double& _x)
{
    if(x.size() > 1)
        std::rotate(x.rbegin(), x.rbegin()+1, x.rend());
    x[0] = _x;
}

void IIR::update_y(const double& _y)
{
    if(y.size() > 1)
        std::rotate(y.rbegin(), y.rbegin()+1, y.rend());
    y[0] = _y;
}

void IIR::set_form(const Form& f)
{
    form = f;
}

IIR::Form IIR::get_form() const
{
    return form;
}

FreqzResult IIR::freqz(int samples /* = 50 */) const
{
    FreqzResult result(samples);
    #pragma omp parallel for
    for(int n_i = 0; n_i < samples; ++n_i){
        const double angle = 2 * PI * n_i/(samples-1);
        std::complex<double> temp_b = (0, 0i);
        #pragma omp parallel for
        for(int n_j = 0; n_j < b.size(); ++n_j){
            temp_b += b[n_j] * (cos(n_j*angle) - 1i *sin(n_j*angle));
        }

        std::complex<double> temp_a = (0, 0i);
        #pragma omp parallel for
        for(int n_j = 0; n_j < a.size(); ++n_j){
            temp_a += a[n_j] * (cos(n_j*angle) - 1i *sin(n_j*angle));
        }

        result.w[n_i] = angle;
        result.h[n_i] = temp_b/temp_a;
    }
    return result;
}
