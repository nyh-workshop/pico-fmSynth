#ifndef MIDITONES_H
#define MIDITONES_H

#include <stdio.h>
#include <cstdint>
#include "midiFreqTable.h"

// Len Shustek's Miditones for arduino:
// https://github.com/LenShustek/arduino-playtune/blob/master/Playtune.cpp

template <typename T>
class PlayTune
{
public:
    PlayTune();
    ~PlayTune();
    void playScore(const uint8_t *inputScore);
    void stopScore();
    void tune_delay(uint32_t msec);
    inline void startPlaying();
    inline bool isPlaying() { return tune_playing; }
    inline void setNoteAdjust(int8_t noteAdj) { noteAdjust = noteAdj; }
    void stepScore();
    
    // static compilation virtual:
    inline void noteOff(uint8_t chan);
    inline void noteOn(uint8_t chan, uint8_t num);
    inline void mutexTryEnter();
    inline void mutexExit();
    inline void sleepMs(uint32_t duration);
    void setUpOneShotTimer(uint32_t durationMs);
    void waitTimerEventComplete();
    
private:
    bool tune_playing = false;
    bool volume_present = false;
    int8_t noteAdjust = 0;
    uint8_t score_start = 0;
    uint8_t* score;
    uint32_t score_cursor;  
};

template <typename T>
inline void PlayTune<T>::noteOff(uint8_t chan) {
    static_cast<T*>(this)->noteOff(chan);
}

template <typename T>
inline void PlayTune<T>::noteOn(uint8_t chan, uint8_t num) {
    static_cast<T*>(this)->noteOn(chan, num);
}

template <typename T>
inline void PlayTune<T>::mutexTryEnter() {
    static_cast<T*>(this)->mutexTryEnter();
}

template <typename T>
inline void PlayTune<T>::mutexExit() {
    static_cast<T*>(this)->mutexExit();
}

template <typename T>
inline void PlayTune<T>::sleepMs(uint32_t duration) {
    static_cast<T*>(this)->sleepMs(duration);
}

template <typename T>
void PlayTune<T>::setUpOneShotTimer(uint32_t durationMs) {
    static_cast<T*>(this)->setUpOneShotTimer(durationMs);
}

template <typename T>
void PlayTune<T>::waitTimerEventComplete() {
     static_cast<T*>(this)->waitTimerEventComplete();
}

template <typename T>
PlayTune<T>::PlayTune() {
    tune_playing = false;
    score_start = 0;
    score_cursor = 0;
    score = nullptr;
}

template <typename T>
PlayTune<T>::~PlayTune() {
    tune_playing = false;
    score_start = 0;
    score_cursor = 0;
    score = nullptr;
}

template <typename T>
inline void PlayTune<T>::startPlaying() {
    tune_playing = true;
}

template <typename T>
void PlayTune<T>::stopScore()
{
    score_start = 0;
    score_cursor = 0;
    tune_playing = false;
}

template <typename T>
void PlayTune<T>::playScore(const uint8_t* inputScore) {

  score = (uint8_t*)inputScore;

  if (tune_playing) stopScore();

#if DBUG
    Serial.print("header: volume_present="); Serial.println(volume_present);
#endif

  tune_playing = true;  /* release the interrupt routine */
}

template <typename T>
void PlayTune<T>::stepScore()
{
    uint8_t cmd, opcode, chan, note;
    uint32_t durationMs;
    /* Do score commands until a "wait" is found, or the score is stopped.
    This is called initially from tune_playcore, but then is called
    from the interrupt routine when waits expire.
  */
    const uint8_t CMD_PLAYNOTE = 0x90;   /* play a note: low nibble is generator #, note is next byte */
    const uint8_t CMD_STOPNOTE = 0x80;   /* stop a note: low nibble is generator # */
    const uint8_t CMD_INSTRUMENT = 0xc0; /* change instrument; low nibble is generator #, instrument is next byte */
    const uint8_t CMD_RESTART = 0xe0;    /* restart the score from the beginning */
    const uint8_t CMD_STOP = 0xf0;       /* stop playing */
    /* if CMD < 0x80, then the other 7 bits and the next byte are a 15-bit big-endian number of msec to wait */

    const uint8_t SCORE_START = 0x00;

    //uint32_t owner;

    while (tune_playing)
    {
        cmd = score[score_cursor++];
        printf("cmd: 0x%x\r\n", cmd);
        if (cmd < 0x80)
        { /* wait count in msec. */
            durationMs = ((unsigned)cmd << 8) | (score[score_cursor++]);

            sleepMs(durationMs);
#if DBUG
            Serial.print("wait ");
            Serial.print(duration);
            Serial.print("ms, cnt ");
            Serial.print(wait_toggle_count);
            Serial.print(" freq ");
            Serial.println(wait_timer_frequency2);
#endif
            break;
        }
        opcode = cmd & 0xf0;
        chan = cmd & 0x0f;
        if (opcode == CMD_STOPNOTE)
        { /* stop note */
            //mutexTryEnter();
            noteOff(chan);            
            // printf("note off event! number: %d\n", chan);
            //mutexExit();

            sleepMs(10);
        }
        else if (opcode == CMD_PLAYNOTE)
        {                                 /* play note */
            note = score[score_cursor++]; // argument evaluation order is undefined in C!
            if (volume_present)
                ++score_cursor; // ignore volume if present

            printf("noteOn: %d, chan: %d\r\n", note, chan);    
			
            //mutexTryEnter();
            noteOn(chan, note);
            //mutexExit();

        }
        else if (opcode == CMD_INSTRUMENT)
        {                   /* change a channel's instrument */
            score_cursor++; // ignore it
        }
        else if (opcode == CMD_RESTART)
        { /* restart score */
            score_cursor = SCORE_START;
        }
        else if (opcode == CMD_STOP)
        { /* stop score */
            tune_playing = false;
            break;
        }
    }
}

#endif