#ifndef FMSYNTH_PICOI2SAUDIODRIVER_H
#define FMSYNTH_PICOI2SAUDIODRIVER_H

#include <Arduino.h>

#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "pico/sem.h"

#include "picoI2sAudio.h"
#include "fmChannel.h"
#include "midiFreqTable.h"

class fmSynthPicoI2s : public PicoI2sAudio<fmSynthPicoI2s> {
    public:
        fmSynthPicoI2s(bool testMode);
        fmSynthPicoI2s(std::string aInstrument);
        fmSynthPicoI2s(uint8_t pinDATAOUT, uint8_t pinBCLK) : PicoI2sAudio<fmSynthPicoI2s>(pinDATAOUT, pinBCLK) {}
        fmSynthPicoI2s(uint8_t pinDATAOUT, uint8_t pinBCLK, std::string aInstrument) : PicoI2sAudio<fmSynthPicoI2s>(pinDATAOUT, pinBCLK)
        {
            Serial1.println("Configure interpolator lanes...");
            configureInterpLanes();
            Serial1.println("Start up fmSynth I2S with user defined pins...");
            for (uint8_t i = 0; i < MAX_FM_CHANNELS; i++)
                fmc[i].setChannelInstrument(aInstrument);
        }
        ~fmSynthPicoI2s();

        // Pico I2S functions:
        int16_t getSample();
        int16_t getSampleTest();

        // Other functions for Pico:
        void configureInterpLanes();

        // Midi functions (velocity not supported for now):
        void midiNoteOff(uint8_t num);
        void midiNoteOn(uint8_t num);

        // Microcontroller SDK specific functions:
        inline void sleepMs(uint32_t ms) { sleep_ms(ms); }

        // Misc. helper functions:
        inline float convertMidiNoteToFreq(uint32_t midiNumber) {return freqOneOctave[midiNumber % 12] * (float)(1 << (uint32_t)(midiNumber / 12));}

    private:
        fmChannel fmc[MAX_FM_CHANNELS];
};

#endif