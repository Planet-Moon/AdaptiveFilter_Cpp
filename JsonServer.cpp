#include "JsonServer.h"
#include <chrono>
#include <thread>

std::string convert_to_string(const Json::Value& json){
    return Json::FastWriter().write(json);
}

JsonServer::JsonServer(int port, const Json::Value& json):
    port(port),  json(json)
{
    svr.Get("/", [this, &json](const httplib::Request &, httplib::Response &res)
    {
        std::string content = convert_to_string(json);
        res.set_content(content, "application/json");
        ++requests_gotten;
    });
}

JsonServer::~JsonServer()
{
    server_thread.join();
}

void JsonServer::host(httplib::Server* svr, int port)
{
    svr->listen("0.0.0.0", port);
}

void JsonServer::host_blocking()
{
    server_thread = std::thread(host, &svr, port);
    while(!is_running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_period));
    }
    while(is_running())
    {
        if(requests_gotten){
            svr.stop();
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_period));
            server_thread.join();
            requests_gotten = 0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_period));
    }
}

bool JsonServer::is_running() const
{
    return svr.is_running();
}
