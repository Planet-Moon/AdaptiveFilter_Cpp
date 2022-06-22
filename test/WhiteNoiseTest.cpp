#include "gtest/gtest.h"
#include <WhiteNoise.h>
#include <Matrix.h>
#include <json/json.h>
#include <httplib.h>
#include <chrono>
#include <thread>

#define SERVER_ACTIVE false

std::string convert_to_string(const Json::Value& json){
    return Json::FastWriter().write(json);
}

TEST(WhiteNoise, Generate){
    WhiteNoise noise(0, 0.1);
    Vec v(1e5);
    for(int i=0; i<v.size(); i++){
        v[i] = noise.generate();
    }

    #if SERVER_ACTIVE
    httplib::Server svr;
    svr.Get("/", [&svr, &v](const httplib::Request &, httplib::Response &res)
    {
        Json::Value result;
        for(int i = 0; i < v.size(); i++){
            result[i] = v[i];
        }
        std::string content = convert_to_string(result);
        res.set_content(content, "application/json");
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        svr.stop();
    });
    svr.listen("0.0.0.0", 8080);
    #endif
}
