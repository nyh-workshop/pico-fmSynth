#ifndef _FMSYNTHCONFIG_H
#define _FMSYNTHCONFIG_H

#include "FpF.hpp"

#if defined(PICO_RP2040)
#define FMSYNTH_SAMPLE_RATE 22050
#elif defined(PICO_RP2350)
#define FMSYNTH_SAMPLE_RATE 44100
#endif

#define PATCH_MAKER_CHANNEL 1

#define MAX_FM_CHANNELS 6

#define SAMPLES_PER_BUFFER 256

typedef mn::MFixedPoint::FpF32<16> fixedPoint;

#endif