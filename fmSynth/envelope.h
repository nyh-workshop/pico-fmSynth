#ifndef _ENVELOPE_H
#define _ENVELOPE_H

#include <math.h>
//#include "FpS.hpp"
//#include "oscillator.h"
#include "fmSynth_main.h"

typedef mn::MFixedPoint::FpF32<16> fp1516;
const uint32_t tenMsTick = (uint32_t)(0.01f * (float)SAMPLE_RATE);

enum EnvelopeState { NONE = 0, ATTACK, DECAY, SUSTAIN, RELEASE };

class Envelope {

public:
	Envelope();
	~Envelope();
	// Split into set levels (L) set rates (R) and set Sustain (sustain):
	// setRates and setLevels are in fixed point!
	// setRatesInSecs and setLevelsInFloat are in floating points!
	void setRatesInSecs(float inputR0_s, float inputR1_s, float inputR3_s);
	void setRates(fp1516 inputR0, fp1516 inputR1, fp1516 inputR3);
	void setSustainInSecs(float inputSustain);
	void setLevelsInFloat(float inputL0_s, float inputL1_s, float inputL3_s);
	void setLevels(fp1516 inputL0, fp1516 inputL1, fp1516 inputL3);
	void setState(EnvelopeState inputState);
	fp1516 envelopeStep();

private:
	fp1516 volume;
	// attack:
	fp1516 L0;
	fp1516 R0;
	// decay:
	fp1516 L1;
	fp1516 R1;
	// sustain (in number of samples):
	uint32_t sustain;
	uint32_t sustainCounter = 0;
	// release:
	fp1516 L3;
	fp1516 R3;

	uint32_t envCount = 0;

	EnvelopeState state = NONE;
};

#endif