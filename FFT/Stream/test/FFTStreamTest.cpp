#include <csignal>
#include <string>
#include <iostream>
#include <json/json.h>
#include <thread>
#include <httplib.h>
#include <WhiteNoise.h>
#include "SinusGenerator.h"


bool running = true;

void signalHandler(int signum) {
    switch (signum) {
        case SIGINT:
            running = false;
            break;
        default:
            break;
    }
}

std::string convert_to_string(const Json::Value& json){
    return Json::FastWriter().write(json);
}

template <typename T>
static Json::Value fromVector(std::vector<T> vector){
    Json::Value result;
    for(int i = 0; i < vector.size(); i++){
        result[i] = vector[i];
    }
    return result;
}


void server_thread_func(std::shared_ptr<httplib::Server> server, const char* address, int port){
    server->listen(address, port);
}

int data_generator(){
    std::shared_ptr<httplib::Server> svr = std::make_shared<httplib::Server>();
    std::shared_ptr<std::vector<float>> value = std::make_shared<std::vector<float>>(0);
    std::shared_ptr<std::vector<float>> time = std::make_shared<std::vector<float>>(0);
    std::shared_ptr<bool> _run = std::make_shared<bool>(false);
    svr->Get("/data", [&value, &time, &_run](const httplib::Request &, httplib::Response &res){
        *_run = true;
        Json::Value json;
        json["time"] = fromVector(*time);
        json["value"] = fromVector(*value);
        std::string content = convert_to_string(json);
        res.set_content(content, "application/json");
        time->clear();
        value->clear();
    });


    float time_step_size = 0.001; // seconds

    SinusGenerator sinusGenerator;
    sinusGenerator.setAmplitude(1);
    sinusGenerator.setFrequency(5);
    sinusGenerator.setSampleTime(time_step_size);
    const float min_freq = 1;
    const float max_freq = 0.5/time_step_size;
    const float freq_speed = (max_freq-min_freq)/100;
    float next_freq = sinusGenerator.getFrequency() + freq_speed;
    bool direction = true;

    WhiteNoise noise(0.f, 0.01f);

    int counter = 0;
    std::thread server_thread(server_thread_func, svr, "0.0.0.0", 8800);
    while(true){
        if(*_run){
            const auto freq = sinusGenerator.getFrequency();
            if(freq >= max_freq){
                direction = false;
            }
            else if(freq <= min_freq){
                direction = true;
            }

            if(next_freq == freq){
                if(direction){
                    next_freq = freq + freq_speed;
                }
                else{
                    next_freq = freq - freq_speed;
                }
            }
            if(next_freq < min_freq){
                next_freq = min_freq;
            }
            else if(next_freq > max_freq){
                next_freq = max_freq;
            }

            if(counter%1000 == 0){
                sinusGenerator.setFrequency(250);//next_freq); // !DEBUG
            }

            float next_value = sinusGenerator.next();//+noise.generate();
            value->push_back(next_value);
            time->push_back(sinusGenerator.getTime());
            if(counter%100 == 0){
                std::cout << "{\"time\":" << time->back() << ",\"value\": " << value->back() << ", \"frequency\":" << freq << "}" << std::endl;
            }
            ++counter;
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>(time_step_size*1000)));
        }
        else{
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    svr->stop();
    server_thread.join();
    return 0;
}

int main(int argc, char **argv){
    return data_generator();
}
