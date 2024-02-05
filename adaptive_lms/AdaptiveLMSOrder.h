#pragma once
#include "MatrixVector.h"
#include <FirOrder.h>
#include <json/json.h>
#include <UpdateStatsOrder.h>


namespace MV = MatrixVector;

template<size_t N>
class AdaptiveLMSOrder: public FirOrder<N>{
public:
    AdaptiveLMSOrder(const MV::Vec<N>& b, double mu): FirOrder<N>(b), mu(mu){}
    virtual ~AdaptiveLMSOrder() = default;

    template<size_t M>
    MV::Vec<N> filter(MV::Vec<N> b, MV::Vec<M> signal) {
        MV::Vec<N> result;
        init_vector(result);
        #pragma omp parallel for
        for(int i = N-1; i < M; ++i){
            double fir_sum = 0;
            #pragma omp parallel for
            for(int n_i = 0; n_i < N; ++n_i){
                fir_sum += signal[i-n_i] * b[n_i];
            }
            result[i] = fir_sum;
        }
        return result;
    }

    UpdateStatsOrder<N> update(double _x, double d) {
        this->update_x(_x);
        y = 0;
        for(int i = 0; i < N; ++i){
            y += this->b[i] * this->x[i];
        }
        e = d - y;
        for(int i = 0; i < N; ++i){
            this->b[i] = this->b[i] + mu * e * this->x[i];
        }

        UpdateStatsOrder<N> us;
        us.b = this->b;
        us.error = e;
        us.y = y;
        return us;
    }

    MV::Vec<N> predict(int samples, int delay) const { return {}; }
    inline double error() const { return e; }

protected:
    double mu;
    double e;
    double y;
};
