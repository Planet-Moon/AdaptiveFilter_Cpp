/*

FIR filter designed with
http://t-filter.appspot.com
http://t-filter.engineerjs.com/

sampling frequency: 2000 Hz

* 0 Hz - 400 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = 4.1393894966071585 dB

* 500 Hz - 1000 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = -40.07355419274887 dB

*/
#include <vector>

const std::vector<double> filter_taps = {
  -0.02010411882885732,
  -0.05842798004352509,
  -0.061178403647821976,
  -0.010939393385338943,
  0.05125096443534972,
  0.033220867678947885,
  -0.05655276971833928,
  -0.08565500737264514,
  0.0633795996605449,
  0.31085440365663597,
  0.4344309124179415,
  0.31085440365663597,
  0.0633795996605449,
  -0.08565500737264514,
  -0.05655276971833928,
  0.033220867678947885,
  0.05125096443534972,
  -0.010939393385338943,
  -0.061178403647821976,
  -0.05842798004352509,
  -0.02010411882885732
};