/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <chrono>
#include <stdio.h>
#include <math.h>

#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/structs/clocks.h"

#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "pico/sem.h"

#include "audio_i2s.h"
#include "sampleMidi.h"

#include "fmSynth/algorithm.h"
#include "fmSynth/fmChannel.h"
#include "fmSynth/FpS.hpp"
#include "fmSynth/oscillator.h"
#include "fmSynth/patch.h"
#include "fmSynth/smallMidiParser.h"
#include "fmSynth/miditones.h"

#define SAMPLES_PER_BUFFER 256

extern "C"
{
    struct audio_buffer_pool *init_audio()
    {
        static audio_format_t audio_format = {
            .sample_freq = 22050,
            .format = AUDIO_BUFFER_FORMAT_PCM_S16,
            .channel_count = 1,
        };

        static struct audio_buffer_format producer_format = {
            .format = &audio_format,
            .sample_stride = 2};

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
}

static mutex_t mPlayer_M;
static semaphore_t c1_S;

const uint8_t maxChannelNum = 6;

fmChannel fm[maxChannelNum] = {fmChannel("E.PIANO"),
                              fmChannel("E.PIANO"),
                              fmChannel("E.PIANO"),
                              fmChannel("E.PIANO"),
                              fmChannel("E.PIANO"),
                              fmChannel("E.PIANO")};

void core1_entry()
{
    printf("Entering core1! :D\n");
    uint32_t owner;
    struct audio_buffer_pool *ap = init_audio();

    sem_release(&c1_S);

    printf("releasing semaphore now!\n");

    while (1)
    {
        if (!mutex_try_enter(&mPlayer_M, &owner))
        {
            printf("at core 1 - locked, owner: %d, core_num: %d\n", owner, get_core_num());
            mutex_enter_blocking(&mPlayer_M);
        }
        struct audio_buffer *buffer = take_audio_buffer(ap, true);
        int16_t *samples = (int16_t *)buffer->buffer->bytes;
        fp1516 tempSample(0.00);

        for (uint i = 0; i < buffer->max_sample_count; i++)
        {

            tempSample = (int16_t)0;
            for(uint8_t chnNum = 0; chnNum < maxChannelNum; chnNum++) {
                tempSample += fm[chnNum].generateSample() * (fp1516)((float)(1.00/maxChannelNum));
            }
            // Set it to 3/4 the max volume!
            samples[i] = (int16_t)( tempSample * (fp1516)(24576.00) );
            
        }
        buffer->sample_count = buffer->max_sample_count;
        give_audio_buffer(ap, buffer);
        mutex_exit(&mPlayer_M);
        //printf("give audio buffer!\n");
    }
}

int main() {
    
    // attempt to overclock to 250MHz: https://www.raspberrypi.org/forums/viewtopic.php?t=304309
    if (!set_sys_clock_khz(270000, false))
      printf("system clock 250MHz failed\n");
    else
      printf("system clock now 250MHz\n");

    stdio_init_all();

    uint32_t owner;
    //smallMidiParser sm(sampleMidi, fm);
    PlayTune mdt(sampleMidiTones, fm);  

    mutex_init(&mPlayer_M);
    sem_init(&c1_S, 0, 2);

    multicore_launch_core1(core1_entry);

    sem_acquire_blocking(&c1_S);

    printf("acquire blocking already!\n");

    mdt.startPlaying();

    // 125MHz:
    // average 3~4 uS for one operator!
    // average 14~16 uS for one FM channel!
    // 250MHz:
    // average 7~8 uS for one FM channel!
    while (true)
    {
        //sm.playMidiData(&midiPlayer_M);
       mdt.stepScore(&mPlayer_M);
    }
    puts("\n");
    return 0;
}
