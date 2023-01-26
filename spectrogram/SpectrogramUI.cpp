#include "SpectrogramUI.h"
#include <Sleep.h>
#include <iostream>

SpectrogramUI::SpectrogramUI(){
    spectrogram = std::make_unique<Spectrogram>(32, 1);
    spectrogram->setEvaluatedSamples(32);
}

SpectrogramUI::~SpectrogramUI(){
    stop();
    run_thread.join();
}

std::vector<SpectrogramUI::Color> SpectrogramUI::vector2Color(const std::vector<double>& vector) const {
    return {};
}

void SpectrogramUI::run(){
    run_thread = std::thread(&SpectrogramUI::_run_func, this);
}

void SpectrogramUI::stop(){
    _run = false;
}

void SpectrogramUI::_run_func(){
    MutexQueue<std::vector<std::complex<double>>>* fft_queue = spectrogram->getFFTQueue();
    auto queue_size = fft_queue->size();


    _run = true;
    std::chrono::steady_clock::time_point _start;
    while(_run){
        _start = std::chrono::steady_clock::now();
        const auto signal = inputThread.next();
        spectrogram->addSample(signal);
        if(fft_queue->size() != queue_size){
            queue_size = fft_queue->size();
        }

        {
            std::lock_guard<std::mutex> guard(*fft_queue->getMutex());
            auto* q = fft_queue->getQueue();
            if(!q->empty())
                q->pop();
        }

        sleep(_start, std::chrono::microseconds(50));
    }
}
