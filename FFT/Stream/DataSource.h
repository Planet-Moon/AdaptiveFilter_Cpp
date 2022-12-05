#pragma once
#include <json/json.h>

class DataSource{
public:
    DataSource();
    virtual ~DataSource() = default;

    virtual Json::Value get_data() = 0;

protected:
    Json::CharReaderBuilder _builder;

    Json::Value parseString(const std::string& str_);

private:
};
