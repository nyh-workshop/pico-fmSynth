#include "smallMidiParser.h"

// MIDI parser based on: http://www.technoblogy.com/list?2EV0

smallMidiParser::smallMidiParser(const uint8_t* inputBuffer, fmChannel* inputFmChannel) {
	midiBuffer = (uint8_t*)inputBuffer;
	pfmc = inputFmChannel;
}

// Flash error light n times: 1=header block, 2=track block, 3=MIDI event
void smallMidiParser::error(uint32_t n) {
	//std::cout << "Error no: " << n << "\n";
}

void smallMidiParser::noteOff(uint8_t number) {
	// noteoff and noteon needs a channel number too!
	uint8_t octave = number / 12;
	uint8_t note = number % 12;
}

// Play note. 60 = middle C (261.63 Hz)
void smallMidiParser::noteOn(uint8_t number) {
	uint8_t octave = number / 12;
	uint8_t note = number % 12;
	//uint32_t freq = Scale[note];
	//uint8_t shift = 9 - octave;
	//Freq[Chan] = freq >> shift;
	//Amp[Chan] = 1 << Decay;
	//Chan = (Chan + 1) & 3;
}

inline float smallMidiParser::convertMidiNoteToFreq(uint32_t midiNumber) {
	return freqOneOctave[midiNumber % 12] * (float)(1 << (uint32_t)(midiNumber / 12)); 
}

void smallMidiParser::playMidiData(mutex_t* pInputMutex) {
	Ptr = 0;                                  // Begin at start of file
	// Read header chunk
	unsigned long type = readNumber(4);
	if (type != MThd) error(1);
	unsigned long len = readNumber(4);
	unsigned int format = readNumber(2);
	unsigned int tracks = readNumber(2);
	unsigned int division = readNumber(2);    // Ticks per beat
	//TempoDivisor = (long)division * 16000 / Tempo;
	TempoDivisor = (float)Tempo / (float)division;
	//std::cout << "Format: " << format << "\n";
	//std::cout << "Division: " << division << "\n";
	//std::cout << "TempoDivisor: " << TempoDivisor << "\n";
	uint32_t owner;

	// Read track chunks
	for (uint32_t t = 0; t < tracks; t++) {
		type = readNumber(4);
		if (type != MTrk) error(2);
		len = readNumber(4);
		EndBlock = Ptr + len;

		// Parse track
		while (Ptr < EndBlock) {
			uint32_t delta = readVariable();
			uint8_t event = readNumber(1);
			uint8_t eventType = event & 0xF0;
			//std::cout << "Delta: " << delta << "\n";
			//std::cout << "Event: " << std::hex << (uint32_t)event << "\n";
			if (delta > 0)
			{

				if (!mutex_try_enter(pInputMutex, &owner))
				{
					printf("at core 0 - locked, owner: %d, core_num: %d\n", owner, get_core_num());
					mutex_enter_blocking(pInputMutex);
				}

				printf("delay: %d\n", (uint32_t)((float)delta * TempoDivisor * 1000000));

				mutex_exit(pInputMutex);

				sleep_us(delta * TempoDivisor * 1000000);
			}

			// Meta event
			if (event == 0xFF) {
				uint8_t mtype = readNumber(1);
				uint8_t mlen = readNumber(1);
				// Tempo
				if (mtype == 0x51) {
					Tempo = readNumber(mlen);
					//printf("Tempo : %d\n", Tempo);
					//TempoDivisor = (uint32_t)division * 16000 / Tempo;
					TempoDivisor = ((float)Tempo / (float)1000000.f) / (float)division;
					//printf("Tempo divisor: %f\n", TempoDivisor);
					// Ignore other meta events
				}
				else readIgnore(mlen);

				// Note off - ignored
			}
			else if (eventType == 0x80) {
				uint8_t channel = event & 0x0f;
				uint8_t number = readNumber(1);
				uint8_t velocity = readNumber(1);
				//noteOff(number);
				//pfmc->noteOff();
				//pfmc[channel].noteOff();
				if (!mutex_try_enter(pInputMutex, &owner))
				{
					printf("at core 0 - locked, owner: %d, core_num: %d\n", owner, get_core_num());
					mutex_enter_blocking(pInputMutex);
				}
				pfmc[channel].noteOff();
				printf("note off event! number: %d\n", number);
				mutex_exit(pInputMutex);
				// Note off
			}
			else if (eventType == 0x90) {
				uint8_t channel = event & 0x0f;
				uint8_t number = readNumber(1);
				uint8_t velocity = readNumber(1);
				//std::cout << "Channel @ 0x90: " << (uint32_t)channel << "\n";
				//pfmc->setFrequency(440.0f * pow(2, ((float)number-69.0f)/12.0f));
				//pfmc->noteOn();
				//pfmc[channel].noteOn();
				if (!mutex_try_enter(pInputMutex, &owner))
				{
					printf("at core 0 - locked, owner: %d, core_num: %d\n", owner, get_core_num());
					mutex_enter_blocking(pInputMutex);
				}
				pfmc[channel].setFrequency(convertMidiNoteToFreq(number));
				pfmc[channel].noteOn();
				printf("note on event! number: %d, frequency: %f\n", number, convertMidiNoteToFreq(number));				
				mutex_exit(pInputMutex);

				// Polyphonic key pressure
			}
			else if (eventType == 0xA0) readIgnore(2);
			// Controller change
			else if (eventType == 0xB0) readIgnore(2);
			// Program change
			else if (eventType == 0xC0) readIgnore(1);
			// Channel key pressure
			else if (eventType == 0xD0) readIgnore(1);
			// Pitch bend
			else if (eventType == 0xE0) readIgnore(2);
			else error(3);
		}
		std::cout << "PlayMidiData done! :D\n";
	}
}

void smallMidiParser::readIgnore(uint32_t n) {
	Ptr = Ptr + n;
}

uint32_t smallMidiParser::readNumber(uint32_t n) {
	uint32_t result = 0;
	for (uint32_t i = 0; i < n; i++) result = (result << 8) + (midiBuffer[Ptr++]);
	return result;
}

uint32_t smallMidiParser::readVariable() {
	uint32_t result = 0;
	uint8_t b;
	do {
		b = (midiBuffer[Ptr++]);
		result = (result << 7) + (b & 0x7F);
	} while (b & 0x80);
	return result;
}
