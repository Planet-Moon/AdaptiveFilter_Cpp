#pragma once
#include <json/json.h>
#include <httplib.h>

class JsonServer{
public:
    JsonServer(int port, const Json::Value& json);
    bool is_running() const;
    void host();
    void host_blocking();
    int sleep_period = 100; // ms
private:
    httplib::Server svr;
    int port = 80;
    Json::Value json;
};
