#include "HTTP_DataSource.h"

HTTP_DataSource::HTTP_DataSource(const std::string& host, const std::string& url):
    DataSource()
{
    _cli = std::make_unique<httplib::Client>(host);
    _url = url;
}

HTTP_DataSource::~HTTP_DataSource(){

}

Json::Value HTTP_DataSource::get_data() {
    auto res = _cli->Get(_url.c_str());
    if(res->status != 200)
        return {};
    Json::Value result = parseString(res->body);
    return result;
};
