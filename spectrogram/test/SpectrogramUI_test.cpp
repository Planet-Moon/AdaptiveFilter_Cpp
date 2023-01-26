#include "gtest/gtest.h"
#include <SpectrogramUI.h>

TEST(SpectrogramUI, One){
    SpectrogramUI specUi;
}

TEST(SpectrogramUI, Two){
    SpectrogramUI specUi;
    specUi.run();
    specUi.stop();
}

TEST(SpectrogramUI, Three){
    SpectrogramUI specUi;
    specUi.spectrogram->setEvaluatedSamples(200);
    specUi.run();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    specUi.stop();
}
