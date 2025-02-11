#ifndef FMSYNTH_PICOI2SAUDIODRIVER_H
#define FMSYNTH_PICOI2SAUDIODRIVER_H

#include <Arduino.h>

#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "pico/sem.h"

#include "picoI2sAudio.h"
#include "miditones.h"
#include "fmChannel.h"

class fmSynthPicoI2s : public PicoI2sAudio<fmSynthPicoI2s>, public PlayTune<fmSynthPicoI2s> {
    public:
        fmSynthPicoI2s(bool testMode);
        ~fmSynthPicoI2s();

        // Pico I2S functions:
        int16_t getSample();
        int16_t getSampleTest();

        //
        void configureInterpLanes();

        // MidiTones functions:
        inline void noteOff(uint8_t chan);
        inline void noteOn(uint8_t chan, uint8_t num);

        // Microcontroller SDK specific functions:
        inline void mutexTryEnter();
        inline void mutexExit();
        inline void sleepMs(uint32_t ms) { sleep_ms(ms); }

        // Misc. helper functions:
        inline float convertMidiNoteToFreq(uint32_t midiNumber) {return freqOneOctave[midiNumber % 12] * (float)(1 << (uint32_t)(midiNumber / 12));}

    private:
        // Microcontroller SDK specific variables:
        mutex_t stepScoreMutex;
        uint32_t owner;
        fmChannel fmc[MAX_FM_CHANNELS];
};

inline void fmSynthPicoI2s::noteOn(uint8_t chan, uint8_t num)
{
    fmc[chan].setFrequency(convertMidiNoteToFreq(num));
    fmc[chan].noteOn();
}

inline void fmSynthPicoI2s::noteOff(uint8_t chan)
{
    fmc[chan].noteOff();
}

#endif