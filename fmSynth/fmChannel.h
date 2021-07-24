#ifndef _FMCHANNEL_H
#define _FMCHANNEL_H

#include "fmSynth_main.h"
//#include "algorithm.h"
#include "oscillator.h"
#include "patch.h"

class fmChannel {
public:
	// Default fm channel settings: all zeros!
	fmChannel();
	fmChannel(std::string patchName);
	//fmChannel(fmPatch inputPatch, fp1516 (*inputAfPtr)(fmChannel*));
	void noteOn();
	void noteOff();
	void setFrequency(float inputFreq);
	uint8_t getFeedback();
	fp1516 generateSample();
	Oscillator osc[MAX_OSC];
private:
	float fmChannelFreq;
	fp1516 fmChannelOutput;	
	fmPatch patch;
	fp1516 (*afPtr)(fmChannel*);
};

#endif
