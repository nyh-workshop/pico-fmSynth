#ifndef PICOI2SAUDIO_H
#define PICOI2SAUDIO_H

#include "fmSynthConfig.h"

#include <I2S.h>
#include <pio_i2s.pio.h>
// Default GPIO pin numbers
#define DEFAULT_pBCLK 31
#define DEFAULT_pWS (DEFAULT_pBCLK+1)
#define DEFAULT_pDOUT 34

extern I2S i2s;

template<typename T>
class PicoI2sAudio {
public:
  PicoI2sAudio();
  PicoI2sAudio(uint8_t pinDATAOUT, uint8_t pinBCLK);
  ~PicoI2sAudio();
  void playSamples();
  int16_t getSample();
protected:
  int16_t buffer[SAMPLES_PER_BUFFER];
};

template<typename T>
PicoI2sAudio<T>::PicoI2sAudio() {
  Serial1.println("Init I2S audio default...");
#if defined(PICO_RP2040)
  Serial1.println("I2S audio for RP2040.");
#elif defined(PICO_RP2350)
  Serial1.println("I2S audio for RP2350.");
#endif
  i2s.setDATA(DEFAULT_pDOUT);
  i2s.setBCLK(DEFAULT_pBCLK); // Note: LRCLK = BCLK + 1
  i2s.setBitsPerSample(16);
  i2s.begin(FMSYNTH_SAMPLE_RATE);
}

template<typename T>
PicoI2sAudio<T>::PicoI2sAudio(uint8_t pinDATAOUT, uint8_t pinBCLK) {
  Serial1.printf("Init I2S audio...");
#if defined(PICO_RP2040)
  Serial1.println("I2S audio for RP2040.");
#elif defined(PICO_RP2350)
  Serial1.println("I2S audio for RP2350.");
#endif
  i2s.setDATA(pinDATAOUT);
  i2s.setBCLK(pinBCLK); // Note: LRCLK = BCLK + 1
  i2s.setBitsPerSample(16);
  i2s.begin(FMSYNTH_SAMPLE_RATE);
}

template<typename T>
PicoI2sAudio<T>::~PicoI2sAudio() {
  Serial1.println("Close I2S audio...");
}

template <typename T>
int16_t PicoI2sAudio<T>::getSample() {
    return static_cast<T*>(this)->getSample();
}

template<typename T>
void PicoI2sAudio<T>::playSamples() {
  int16_t b = 0;
  for (uint i = 0; i < SAMPLES_PER_BUFFER; i++) {
    // absolute_time_t before = get_absolute_time();
    b = getSample();
    i2s.write(b);
    i2s.write(b);
    // absolute_time_t after = get_absolute_time();
    // printf("time to generate sample: %d\n", (int32_t)absolute_time_diff_us(before, after));
  }
}

#endif