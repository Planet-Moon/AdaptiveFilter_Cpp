#pragma once
#include <string>
#include <MatrixVector.h>
#include <FreqzResult.h>

const double PI = std::acos(-1);
using namespace std::complex_literals;

namespace MV = MatrixVector;

template<size_t N>
class FirOrder {
public:
    FirOrder(const MV::Vec<N>& b): b(b) {
        init_vector(x, 0.0);
    }
    virtual ~FirOrder() = default;

    double filter(const double& input) {
        update_x(input);

        double y = 0.0;
        for(unsigned int i = 0; i < N; ++i){
            y += x[i] * b[i];
        }
        return y;
    }

    std::string impz() const {
        std::string result = "";
        for(unsigned int i = 0; i < N; ++i){
            if(b[i] == 0) continue;
            result += std::to_string(b[i]) + "*z^-" + std::to_string(i);
            if(i+1 < N && b[i+1] >= 0)
                result += " + ";
        }
        return result;
    }

    inline MV::Vec<N> get_b() const { return b; }
    constexpr unsigned int get_n() const { return N; };

    FreqzResult freqz(int samples = 50){
        return freqz(this->b, samples);
    }

    static FreqzResult freqz(const MV::Vec<N>& _b, int samples = 50){
        FreqzResult result(samples);
        #pragma omp parallel for
        for(int n_i = 0; n_i < samples; ++n_i){
            const double angle = 2 * PI * n_i/(samples-1);
            std::complex<double> temp = 0.0 + 0i;
            #pragma omp parallel for
            for(int n_j = 0; n_j < N; ++n_j){
                temp += _b[n_j] * (cos(n_j*angle) - 1i * sin(n_j*angle));
            }
            result.w[n_i] = angle;
            result.h[n_i] = temp;
        }
        return result;
    }

protected:
    FirOrder() {
        init_vector(x, 0.0);
    }
    MV::Vec<N> b;
    MV::Vec<N> x;
    virtual void update_x(const double& _x) final {
        if(x.size() > 1)
            std::rotate(x.rbegin(), x.rbegin()+1, x.rend());
        x[0] = _x;
    }
};
