#ifndef _FMSYNTH_MAIN_H
#define _FMSYNTH_MAIN_H

#include "FpF.hpp"

#define SAMPLES_PER_BUFFER 256

const int32_t SAMPLE_RATE = 22050;

#define NO_DEBUG

//#define SINE_DEBUG

//#define PATCH_DEBUG

#ifdef NO_DEBUG
const uint8_t MAX_FM_CHANNELS = 6;
#elif defined SINE_DEBUG
const uint8_t MAX_FM_CHANNELS = 1;
#elif defined PATCH_DEBUG
const uint8_t MAX_FM_CHANNELS = 1;
#else
const uint8_t MAX_FM_CHANNELS = 1;
#endif

// Buffer length = 2 * working buffers. This is for the audio DAC where there are left + right channels.
#define BUFFER_LENGTH 512 

typedef mn::MFixedPoint::FpF32<16> fixedPoint;

#endif