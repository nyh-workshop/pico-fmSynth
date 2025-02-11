#ifndef PICOI2SAUDIO_H
#define PICOI2SAUDIO_H

#include "fmSynthConfig.h"

#if defined(RP2040_SDK)
#include "audio_i2s.h"
#include "stdio.h"
#include "pico/stdlib.h"
#elif defined(RP2350_ARDUINO)
#include <I2S.h>
#include <pio_i2s.pio.h>
// GPIO pin numbers
#define pBCLK 31
#define pWS (pBCLK+1)
#define pDOUT 34

extern I2S i2s;
#endif

template<typename T>
class PicoI2sAudio {
public:
  PicoI2sAudio();
  ~PicoI2sAudio();
  void playSamples();
  int16_t getSample();
protected:
#if defined(RP2040_SDK)
  struct audio_buffer_pool* init_audio();
  struct audio_buffer_pool* ap = nullptr;
#elif defined(RP2350_ARDUINO)
  int16_t buffer[SAMPLES_PER_BUFFER];
#endif
};

template<typename T>
PicoI2sAudio<T>::PicoI2sAudio() {
  Serial1.println("Init I2S audio...");
#if defined(RP2040_SDK)
  ap = init_audio();
#elif defined(RP2350_ARDUINO)
  i2s.setDATA(pDOUT);
  i2s.setBCLK(pBCLK); // Note: LRCLK = BCLK + 1
  i2s.setBitsPerSample(16);
  i2s.begin(FMSYNTH_SAMPLE_RATE);
#endif
}

template<typename T>
PicoI2sAudio<T>::~PicoI2sAudio() {
#if defined(RP2040_SDK)
  ap = nullptr;
#elif defined(RP2350_ARDUINO)
  Serial1.println("Close I2S audio...");
#endif
}

#if defined(RP2040_SDK)
template<typename T>
struct audio_buffer_pool *PicoI2sAudio<T>::init_audio() {
  static audio_format_t audio_format = {
    .sample_freq = SAMPLE_RATE,
    .format = AUDIO_BUFFER_FORMAT_PCM_S16,
    .channel_count = 1,
  };

  static struct audio_buffer_format producer_format = {
    .format = &audio_format,
    .sample_stride = 2
  };

  struct audio_buffer_pool *producer_pool = audio_new_producer_pool(&producer_format, 3, SAMPLES_PER_BUFFER);  // todo correct size
  bool __unused ok;
  const struct audio_format *output_format;
  struct audio_i2s_config config = {
    .data_pin = PICO_AUDIO_I2S_DATA_PIN,
    .clock_pin_base = PICO_AUDIO_I2S_CLOCK_PIN_BASE,
    .dma_channel = 0,
    .pio_sm = 0,
  };

  output_format = audio_i2s_setup(&audio_format, &config);
  if (!output_format) {
    panic("PicoAudio: Unable to open audio device.\n");
  }

  ok = audio_i2s_connect(producer_pool);
  assert(ok);
  audio_i2s_set_enabled(true);
  return producer_pool;
}
#endif

template <typename T>
int16_t PicoI2sAudio<T>::getSample() {
    return static_cast<T*>(this)->getSample();
}

template<typename T>
void PicoI2sAudio<T>::playSamples() {
#if defined(RP2040_SDK)
  struct audio_buffer *buffer = take_audio_buffer(ap, true);
  int16_t *samples = (int16_t *)buffer->buffer->bytes;

  for (uint i = 0; i < buffer->max_sample_count; i++) {
    // absolute_time_t before = get_absolute_time();
    samples[i] = getSample();
    // absolute_time_t after = get_absolute_time();
    // printf("time to generate sample: %d\n", (int32_t)absolute_time_diff_us(before, after));
  }

  buffer->sample_count = buffer->max_sample_count;
  give_audio_buffer(ap, buffer);

#elif defined(RP2350_ARDUINO)

  int16_t b = 0;
  for (uint i = 0; i < SAMPLES_PER_BUFFER; i++) {
    // absolute_time_t before = get_absolute_time();
    b = getSample();
    i2s.write(b);
    i2s.write(b);
    // absolute_time_t after = get_absolute_time();
    // printf("time to generate sample: %d\n", (int32_t)absolute_time_diff_us(before, after));
  }

#endif
}

#endif