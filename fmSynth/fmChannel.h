#ifndef _FMCHANNEL_H
#define _FMCHANNEL_H

#include "oscillator.h"
#include "patch.h"

class fmChannel {
public:
	using algorithmPtr = int32_t (fmChannel::*)();

	// Lumping the algorithms into the FM channel:
	int32_t algorithm0();
	int32_t algorithm1();
	int32_t algorithm2();
	int32_t algorithm3();
	int32_t algorithm4();
	int32_t algorithm5();
	int32_t algorithm6();
	int32_t algorithm7();
	int32_t algorithm8();

	// Default fm channel settings: all zeros!
	fmChannel();
	fmChannel(std::string patchName);
	void noteOn();
	void noteOff();
	void setFrequency(float inputFreq);
	uint8_t getFeedback();
	int32_t generateSample();

	// Temporary fix for this one - 4 oscillators maximum for each FM channel!
	static const uint8_t MAX_OSC = 4;
	Oscillator osc[MAX_OSC];
private:
	float fmChannelFreq;
	int32_t fmChannelOutput;
	fmPatch patch;

	// Algorithm0 is for testing only - one sine oscillator! :D
	int32_t (fmChannel::*a0)() = &fmChannel::algorithm0;
	int32_t (fmChannel::*a2)() = &fmChannel::algorithm2;
	int32_t (fmChannel::*a3)() = &fmChannel::algorithm3;
	int32_t (fmChannel::*a5)() = &fmChannel::algorithm5;
	int32_t (fmChannel::*a7)() = &fmChannel::algorithm7;
	
	//https://stackoverflow.com/questions/57926015/how-to-declare-an-array-of-pointers-to-member-functions-in-c
	static constexpr algorithmPtr fnArray[8] = { &fmChannel::algorithm0, nullptr, &fmChannel::algorithm2, &fmChannel::algorithm3, nullptr, &fmChannel::algorithm5, nullptr, &fmChannel::algorithm7 };

	algorithmPtr afPtr;

	//int32_t (*afPtr)(fmChannel*);
};

#endif
