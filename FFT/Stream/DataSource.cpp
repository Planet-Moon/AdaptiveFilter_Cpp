#include "DataSource.h"

Json::Value parseString(const std::string& str_, const Json::CharReaderBuilder& _builder){
    Json::Value result;
    Json::String errs;
    std::stringstream ss;
    ss.str(str_);
    bool ok = Json::parseFromStream(_builder, ss, &result, &errs);
    return result;
}

DataSource::DataSource(){
    _builder["collectComments"] = false;
};


Json::Value DataSource::parseString(const std::string& str_){
    return ::parseString(str_, _builder);
}
