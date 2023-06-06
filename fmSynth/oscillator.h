#ifndef _OSCILLATOR_H
#define _OSCILLATOR_H

#include "envelope.h"

// https://dev.to/noah11012/start-using-asserts-452c
//#define assert(expression) if(expression == false) printf("assert failed!\n"); exit(1)

class Oscillator {
public:
	Oscillator();
	~Oscillator();
	void setFrequency(float inputFreq);
	inline void setTuningWord(uint32_t inputTuningWord) { tuningWord = inputTuningWord; }
	
	// opfb can take fbShift if needed feedback into self.
	int32_t opfb(uint8_t fbShift);
	int32_t op(int32_t inputFeedback);

	int32_t opSineTest();
	int32_t opSineFbTest(uint8_t fbShift);

	void clearFeedbackArray();
	
	void configureInterpLanes();
	Envelope<fixedPoint> adsr;
private:
	int32_t feedback[2] = {0, 0};
	uint32_t tuningWord;
	uint32_t accumulator;

	static bool wavetableFilled;
};

#endif