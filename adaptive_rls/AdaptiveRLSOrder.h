#pragma once
#include "FirOrder.h"
#include "MatrixVector.h"
#include <UpdateStatsOrder.h>
#include <cassert>

namespace MV = MatrixVector;

template<size_t N>
class AdaptiveRLSOrder: public FirOrder<N> {
public:
    AdaptiveRLSOrder(double p = 0.95): FirOrder<N>(), p(p){
        init_vector(this->b, 0.);
        R_inv = MV::Matrix::identity<N>();
        e = 0.;
        init_vector(z, 0.);
        y = 0.;
    }

    template<size_t M>
    MV::Vec<M> filter(MV::Vec<N> b, MV::Vec<M> signal) {
        auto result = MV::create_vector<M>();
        #pragma omp parallel for
        for(int i = N-1; i < M; ++i){
            double fir_sum = 0.;
            #pragma omp parallel for
            for(int n_i = 0; n_i < N; ++n_i){
                fir_sum += signal[i-n_i] * b[n_i];
            }
            result[i] = fir_sum;
        }
        return result;
    }

    UpdateStatsOrder<N> update(double _x, double d){
        this->update_x(_x);
        MV::Mat<1, N> x_m = MV::Matrix::fromVector(this->x);
        MV::Vec<1> y_v = x_m * this->b;
        double y = y_v[0];
        e = d - y;
        MV::Vec<N> z;
        {
            MV::Vec<N> numerator = R_inv * this->x;
            MV::Vec<1> denominator_p = x_m * numerator;
            double denominator = p + denominator_p[0];
            assert(denominator != 0);
            z = numerator * (1/denominator);
        }
        this->b = this->b + e*z;
        MV::Mat<1,N> z1 = x_m * R_inv;
        MV::Mat<1,N> z_m = MV::Matrix::fromVector(z);
        MV::Mat<N,1> z_m_t;
        MV::Matrix::transpose(z_m, z_m_t);
        MV::Mat<N,N> z2 = z_m_t * z1;
        MV::Mat z3 = R_inv - z2;
        R_inv = 1/p * z3;
        UpdateStatsOrder<N> us;
        us.b = this->b;
        us.error = abs(this->e);
        us.y = y;
        return us;
    }

    template<size_t M>
    MV::Vec<N+M> predict(int delay) const {
        auto result = MV::create_vector<M + N>();
        #pragma omp parallel for
        for(int i = 0; i < N; ++i){
            const MV::Vec<N> local_x = MV::create_vector<N>();
            auto z = MV::Matrix::fromVector(local_x) * this->b;
            result[i] = z[0];
        }
        return result;
    }

    inline double error() const {
        return e;
    }

private:
    double p;
    double e;
    MV::Vec<N> z;
    double y;
    MV::Mat<N, N> R_inv;
};
