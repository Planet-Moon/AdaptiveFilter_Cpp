#pragma once
#include <MatrixVector.h>
#include <json/json.h>

namespace MV = MatrixVector;

template<size_t N>
struct UpdateStatsOrder{
    double error;
    double y;
    MV::Vec<N> b;
    Json::Value toJson() const {
        Json::Value result;
        result["error"] = error;
        Json::Value bJson{};
        for(int i=0; i<N; ++i){
            bJson[i] = b[i];
        }
        result["b"] = bJson;
        result["y"] = y;
        return result;
    }
};
