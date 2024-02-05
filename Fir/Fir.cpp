#include "Fir.h"
#include <algorithm>
#include <cmath>

const double PI = std::acos(-1);
using namespace std::complex_literals;

Fir::Fir(const Vec& b): b(b), n(b.size())
{
    x = Vec(n, 0);
}

Fir::Fir(unsigned int& n): n(n)
{
    b = Vec(n, 0);
    x = Vec(n, 0);
}

void Fir::update_x(const double& _x)
{
    if(x.size() > 1)
        std::rotate(x.rbegin(), x.rbegin()+1, x.rend());
    x[0] = _x;
}

double Fir::filter(const double& input)
{
    update_x(input);

    double y = 0.0;
    for(unsigned int i = 0; i < n; i++){
        y += x[i] * b[i];
    }
    return y;
}

std::string Fir::impz() const
{
    std::string result = "";
    for(unsigned int i = 0; i < n; ++i){
        if(b[i] == 0) continue;
        result += std::to_string(b[i]) + "*z^-" + std::to_string(i);
        if(i+1 < n && b[i+1] >= 0)
            result += " + ";
    }
    return result;
}

Vec Fir::get_b() const
{
    return b;
}

unsigned int Fir::get_n() const
{
    return n;
}

FreqzResult Fir::freqz(int samples /* = 50 */) const
{
    return freqz(b, samples);
}

FreqzResult Fir::freqz(const Fir& fir, int samples /* = 50 */)
{
    return freqz(fir.get_b(), samples);
}

FreqzResult Fir::freqz(const Vec& b, int samples /* = 50 */)
{
    int n = b.size();
    FreqzResult result(samples);
    #pragma omp parallel for
    for(int n_i = 0; n_i < samples; ++n_i){
        const double angle = 2 * PI * n_i/(samples-1);
        std::complex<double> temp = 0.0 + 0i;
        #pragma omp parallel for
        for(int n_j = 0; n_j < n; ++n_j){
            temp += b[n_j] * (cos(n_j*angle) - 1i *sin(n_j*angle));
        }
        result.w[n_i] = angle;
        result.h[n_i] = temp;
    }
    return result;
}
