#ifndef _OSCILLATOR_H
#define _OSCILLATOR_H

#include "envelope.h"
//#include "wavetable.h"
#include "fmSynth_main.h"

const fp1516 oscPiFP(M_PI);
const fp1516 sampleRateFP(SAMPLE_RATE);
const fp1516 sampleRateFP_r((float)1/(float)SAMPLE_RATE);

// https://dev.to/noah11012/start-using-asserts-452c
//#define assert(expression) if(expression == false) printf("assert failed!\n"); exit(1)

class Oscillator {
public:
	Oscillator();
	~Oscillator();
	void setFrequency(float inputFreq);
	inline fp1516 calculateSine(fp1516 inputFP);
	inline fp1516 getAccumulator() { return accumulatorFP;  }
	//inline uint32_t getAccumulator() { return accumulator; }
	inline void setTempFP(fp1516 inputTempFP) { tempFP = inputTempFP;  }
	fp1516& setTempFP();
	inline fp1516 getTempFP() { return tempFP; }
	// opfb can take fbShift if needed feedback into self.
	fp1516 opfb(uint8_t fbShift);
	fp1516 op(fp1516 inputFeedback);
	Envelope adsr;
private:	
	inline fp1516 bhaskaraOneSine(fp1516 inputFP);
	fp1516 feedbackFP[4] = { 0.0, 0.0, 0.0, 0.0 }; // feedback buffers (for Yamaha FM feedback style) 
	fp1516 freqFP;
	fp1516 tempFP; // for storing feedback data when doing each sample!
	fp1516 accumulatorFP;
	uint32_t tuningWord;
	uint32_t accumulator;
	int32_t sampleTime;
};

#endif