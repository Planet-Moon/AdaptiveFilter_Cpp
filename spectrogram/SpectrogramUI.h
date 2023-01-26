#pragma once
#include <InputThread.h>
#include <Spectrogram.h>


class SpectrogramUI{
public:
    SpectrogramUI();
    virtual ~SpectrogramUI();

    void run();
    void stop();

    struct Color{
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
    };
    std::vector<Color> vector2Color(const std::vector<double>& vector) const;

    InputThread inputThread;
    std::unique_ptr<Spectrogram> spectrogram;

private:
    bool _run{true};

    std::thread run_thread;
};
