#ifndef _OSCILLATOR_H
#define _OSCILLATOR_H

#include <Arduino.h>

// Interpolator for Oscillator's op functions:
#include "hardware/interp.h"

#include "envelope.h"
#include "fmSynthConfig.h"

class Oscillator {
public:
	Oscillator();
	~Oscillator();
	void setFrequency(float inputFreq);
	inline void setTuningWord(uint32_t inputTuningWord) { tuningWord = inputTuningWord; }
	
	// opfb can take fbShift if needed feedback into self.
	int32_t opfb(uint8_t fbShift);
	int32_t op(int32_t inputFeedback);

  // Convenient to have if we have to port it to different platforms!
	int32_t opSineTest();
	int32_t opSineFbTest(uint8_t fbShift);

	void clearFeedbackArray();
	
  #if defined(RP2040_ARDUINO)
	Envelope<fixedPoint> adsr;
  #elif defined(RP2350_ARDUINO)
  Envelope<float> adsr;
  #endif
private:
	int32_t feedback[2] = {0, 0};
	uint32_t tuningWord;
	uint32_t accumulator;

	static bool wavetableFilled;
};

#endif