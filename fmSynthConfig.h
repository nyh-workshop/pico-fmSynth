#ifndef _FMSYNTHCONFIG_H
#define _FMSYNTHCONFIG_H

#include "FpF.hpp"

// Select your Raspberry Pico here:
// #define RP2350_ARDUINO
#define RP2040_ARDUINO

#if defined(RP2040_ARDUINO)
#define FMSYNTH_SAMPLE_RATE 22050
#elif defined(RP2350_ARDUINO)
#define FMSYNTH_SAMPLE_RATE 44100
#endif

#define MAX_FM_CHANNELS 6

#define SAMPLES_PER_BUFFER 256

typedef mn::MFixedPoint::FpF32<16> fixedPoint;

#endif