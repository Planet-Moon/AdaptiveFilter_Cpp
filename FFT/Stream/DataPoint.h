#pragma once
#include <vector>
#include <json/json.h>

struct DataPoint{
    DataPoint();
    DataPoint(const float& t, const float& v);
    float time;
    float value;

    static std::vector<DataPoint> fromJson(const Json::Value& jv);

    template<typename T>
    static std::vector<T> toVec(const std::vector<DataPoint>& v){
        std::vector<T> result;
        for(const auto& i : v){
            result.push_back(static_cast<T>(i.value));
        }
        return result;
    }
};
