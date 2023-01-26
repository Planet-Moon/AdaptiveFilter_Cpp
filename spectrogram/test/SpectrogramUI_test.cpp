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
}
