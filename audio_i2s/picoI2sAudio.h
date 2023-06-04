#ifndef PICOI2SAUDIO_H
#define PICOI2SAUDIO_H

#include "audio_i2s.h"
#include "stdio.h"
#include "pico/stdlib.h"

#define SAMPLES_PER_BUFFER 128
#define SAMPLE_RATE 22050

template <typename T>
class PicoI2sAudio {
    public:
        PicoI2sAudio();
        ~PicoI2sAudio();
        void playSamples();
        int16_t getSample();
    protected:
        struct audio_buffer_pool* init_audio();
        struct audio_buffer_pool* ap = nullptr;
};

template <typename T>
PicoI2sAudio<T>::PicoI2sAudio() {
    printf("Init I2S audio...\n");
    ap = init_audio();
}

template <typename T>
PicoI2sAudio<T>::~PicoI2sAudio() {
    ap = nullptr;
}

template <typename T>
struct audio_buffer_pool* PicoI2sAudio<T>::init_audio()
{
    static audio_format_t audio_format = {
        .sample_freq = SAMPLE_RATE,
        .format = AUDIO_BUFFER_FORMAT_PCM_S16,
        .channel_count = 1,
    };

    static struct audio_buffer_format producer_format = {
        .format = &audio_format,
        .sample_stride = 2
        };

    struct audio_buffer_pool *producer_pool = audio_new_producer_pool(&producer_format, 3, SAMPLES_PER_BUFFER); // todo correct size
    bool __unused ok;
    const struct audio_format *output_format;
    struct audio_i2s_config config = {
        .data_pin = PICO_AUDIO_I2S_DATA_PIN,
        .clock_pin_base = PICO_AUDIO_I2S_CLOCK_PIN_BASE,
        .dma_channel = 0,
        .pio_sm = 0,
    };

    output_format = audio_i2s_setup(&audio_format, &config);
    if (!output_format)
    {
        panic("PicoAudio: Unable to open audio device.\n");
    }

    ok = audio_i2s_connect(producer_pool);
    assert(ok);
    audio_i2s_set_enabled(true);
    return producer_pool;
}

template <typename T>
int16_t PicoI2sAudio<T>::getSample() {
    return static_cast<T*>(this)->getSample();
}

template <typename T>
void PicoI2sAudio<T>::playSamples()
{
    struct audio_buffer *buffer = take_audio_buffer(ap, true);
    int16_t *samples = (int16_t *)buffer->buffer->bytes;

    for (uint i = 0; i < buffer->max_sample_count; i++)
    {
        // absolute_time_t before = get_absolute_time();
        samples[i] = getSample();
        // absolute_time_t after = get_absolute_time();
        // printf("time to generate sample: %d\n", (int32_t)absolute_time_diff_us(before, after));
    }
    buffer->sample_count = buffer->max_sample_count;
    give_audio_buffer(ap, buffer);
}

#endif