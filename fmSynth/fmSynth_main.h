#ifndef _FMSYNTH_MAIN_H
#define _FMSYNTH_MAIN_H

#include "FpS.hpp"

#include <chrono>
#include <iostream>
#include <map>
#include <math.h>
#include <stdio.h>
#include <cstring>

#include <pico.h>

#include "pico/stdlib.h"

const int32_t SAMPLE_RATE = 22050;
// Buffer length = 2 * working buffers. This is for the audio DAC where there are left + right channels.
#define BUFFER_LENGTH 512 

const uint8_t MAX_OSC = 4;

typedef mn::MFixedPoint::FpF32<16> fp1516;

#endif