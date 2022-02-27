#ifndef _FMSYNTH_MAIN_H
#define _FMSYNTH_MAIN_H

#include "FpF.hpp"

const int32_t SAMPLE_RATE = 22050;

const uint8_t MAX_FM_CHANNELS = 6;

// Buffer length = 2 * working buffers. This is for the audio DAC where there are left + right channels.
#define BUFFER_LENGTH 512 

typedef mn::MFixedPoint::FpF32<16> fixedPoint;

#endif