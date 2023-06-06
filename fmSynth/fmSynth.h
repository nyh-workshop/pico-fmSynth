#ifndef _FMSYNTH_H
#define _FMSYNTH_H

#include "FpF.hpp"

#include <math.h>

// Interpolator for Oscillator's op functions:
#include "hardware/interp.h"

#include "pico/stdlib.h"

constexpr int32_t FMSYNTH_SAMPLE_RATE = 22050;

constexpr uint8_t MAX_FM_CHANNELS = 6;

// Buffer length = 2 * working buffers. This is for the audio DAC where there are left + right channels.
#define BUFFER_LENGTH 512 

typedef mn::MFixedPoint::FpF32<16> fixedPoint;

#endif