#include <csignal>
#include <string>
#include <iostream>
#include <json/json.h>
#include <thread>
#include <httplib.h>


const float M_PI = acos(-1);

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
class SinusGenerator{
public:
    SinusGenerator(){}

    float getFrequency() const { return _freq; }
    void setFrequency(float frequency) { _freq = frequency; }

    float getAmplitude() const { return _amp; }
    void setAmplitude(float amplitude) { _amp = amplitude; }

    float getSampleTime() const { return _sample_time; }
    void setSampleTime(float sample_time) { _sample_time = sample_time; }

    float getTime() const { return _continuos_time; }

    float next(){
        float res = _amp * sin(2 * M_PI * _freq * _timestep);
        _timestep += _sample_time;
        if(_timestep > 1/_freq){
            int ratio = _timestep/_sample_time;
            _timestep = _sample_time*(_timestep/_sample_time - ratio);
        }
        _continuos_time += _sample_time;
        return res;
    }

private:
    float _freq=1;
    float _amp=1;

    float _timestep=0;
    float _sample_time=100;
    float _continuos_time=0;
};

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

int main(int argc, char **argv){
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


    SinusGenerator sinusGenerator;
    sinusGenerator.setAmplitude(1);
    sinusGenerator.setFrequency(1);
    sinusGenerator.setSampleTime(0.01);
    int counter = 0;
    std::thread server_thread(server_thread_func, svr, "0.0.0.0", 8800);
    while(true){
        if(*_run){
            value->push_back(sinusGenerator.next());
            time->push_back(sinusGenerator.getTime());
            std::cout << time->back() << ": " << value->back() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    svr->stop();
    server_thread.join();
    return 0;
}
