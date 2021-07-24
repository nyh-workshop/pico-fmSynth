#ifndef SMALLMIDIPARSER_H
#define SMALLMIDIPARSER_H

#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "pico/sem.h"
#include "pico/stdlib.h"

#include "fmSynth_main.h"
#include "fmChannel.h"
#include "midiFreqTable.h"

// MIDI parser based on: http://www.technoblogy.com/list?2EV0
class smallMidiParser
{

public:
	smallMidiParser(const uint8_t* inputBuffer, fmChannel* inputFmChannel);
	void error(uint32_t n);
	void noteOn(uint8_t number);
	void noteOff(uint8_t number);
	void playMidiData(mutex_t* pInputMutex);
	void readIgnore(uint32_t n);
	inline float convertMidiNoteToFreq(uint32_t midiNumber);
	uint32_t readNumber(uint32_t n);
	uint32_t readVariable();

private:
	uint32_t Ptr = 0;
	uint32_t EndBlock = 0;
	uint32_t Tempo = 500000;    // Default - microseconds per beat; ie 120bpm
	float TempoDivisor = 1.00f;
	static const uint32_t MThd = 0x4D546864;
	static const uint32_t MTrk = 0x4D54726B;
	fmChannel* pfmc;

	uint8_t* midiBuffer;
};

#endif
