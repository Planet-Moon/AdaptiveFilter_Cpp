#include "DataPoint.h"

DataPoint::DataPoint():time(0.f),value(0.f){

};

DataPoint::DataPoint(const float& t, const float& v): time(t), value(v) {

};

std::vector<DataPoint> DataPoint::fromJson(const Json::Value& jv){
    const Json::Value& _time = jv["time"];
    const Json::Value& _value = jv["value"];
    Json::ArrayIndex size = _time.size();
    std::vector<DataPoint> result;
    for(int i = 0; i < size; ++i){
        DataPoint p(_time[i].asFloat(), _value[i].asFloat());
        result.push_back(p);
    }
    // time = jv.get("time",0).asFloat();
    // value = jv.get("value",0).asFloat();
    return result;
}
