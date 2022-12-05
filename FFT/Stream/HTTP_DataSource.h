#pragma once
#include "DataSource.h"
#include <httplib.h>
#include <memory>

class HTTP_DataSource : public DataSource {
public:
    HTTP_DataSource(const std::string& host, const std::string& url);

    virtual ~HTTP_DataSource();

    Json::Value get_data() override;

private:
    std::unique_ptr<httplib::Client> _cli;
    std::string _url;
};
