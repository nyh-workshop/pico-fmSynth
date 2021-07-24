#ifndef MIDITONES_H
#define MIDITONES_H

#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "pico/sem.h"
#include "pico/stdlib.h"

#include "fmSynth_main.h"
#include "fmChannel.h"
#include "midiFreqTable.h"

// Len Shustek's Miditones for arduino:
// https://github.com/LenShustek/arduino-playtune/blob/master/Playtune.cpp

class PlayTune
{
public:
    PlayTune(const uint8_t* inputScore, fmChannel* inputFmChannel);
    void playScore(const uint8_t *inputScore);
    void stopScore();
    void tune_delay(unsigned msec);
    void stepScore(mutex_t* pInputMutex);
    inline float convertMidiNoteToFreq(uint32_t midiNumber) {return freqOneOctave[midiNumber % 12] * (float)(1 << (uint32_t)(midiNumber / 12));}
    inline void startPlaying() { tune_playing = true; }

private:
    bool tune_playing = false;
    bool volume_present = false;
    const uint8_t score_start = 0;
    fmChannel* pfmc;
    uint8_t* score;
    uint32_t score_cursor;    
};

#endif