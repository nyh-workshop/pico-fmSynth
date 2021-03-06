/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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

#include "fmSynth/miditones.h"

#include "fmSynth/fmSynth_main.h"
#include "fmSynth/fmPatchTuner.h"

void configureInterpLanes() {
	interp_config interpCfg = interp_default_config();

	// Lane 0 settings:
    interp_config_set_add_raw(&interpCfg, true);
    interp_config_set_cross_input(&interpCfg, 0);
    interp_config_set_mask(&interpCfg, 22, 31);
    interp_config_set_shift(&interpCfg, 0);
    interp_config_set_signed(&interpCfg, false);
    interp_set_config(interp1, 0, &interpCfg);
    // Lane 1 settings:
    interp_config_set_add_raw(&interpCfg, true);
    interp_config_set_cross_input(&interpCfg, 0);
    interp_config_set_mask(&interpCfg, 0, 9);
    interp_config_set_shift(&interpCfg, 22);
    interp_config_set_signed(&interpCfg, false);
    interp_set_config(interp1, 1, &interpCfg);
}

extern "C"
{
    struct audio_buffer_pool *init_audio()
    {
        static audio_format_t audio_format = {
            .sample_freq = SAMPLE_RATE,
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

#ifdef NO_DEBUG
fmChannel fm[MAX_FM_CHANNELS] = {fmChannel("E.PIANO"),
                               fmChannel("E.PIANO"),
                               fmChannel("E.PIANO"),
                               fmChannel("E.PIANO"),
                               fmChannel("E.PIANO"),
                               fmChannel("E.PIANO")};
#elif defined SINE_DEBUG
fmChannel fm[1] = {fmChannel("TEST00")};
#endif

fmChannel fm_patchDebug[1] = {fmChannel("FLUTE")};

void core1_entry()
{
    printf("Entering core1! :D\n");
    uint32_t owner;
    struct audio_buffer_pool *ap = init_audio();

    // RP2040's interpolator module needs to be configured at the respective core before running.
    configureInterpLanes();

    sem_release(&c1_S);

    printf("releasing semaphore now!\n");

    while (1)
    {
        if (!mutex_try_enter(&mPlayer_M, &owner))
        {
            //printf("at core 1 - locked, owner: %d, core_num: %d\n", owner, get_core_num());
            mutex_enter_blocking(&mPlayer_M);
        }
        struct audio_buffer *buffer = take_audio_buffer(ap, true);
        int16_t* samples = (int16_t*)buffer->buffer->bytes;
        int16_t tempSample = 0;

        for (uint i = 0; i < buffer->max_sample_count; i++)
        {
            tempSample = (int16_t)0;
            for(uint8_t chnNum = 0; chnNum < MAX_FM_CHANNELS; chnNum++) {
                //absolute_time_t before = get_absolute_time();
                #ifdef NO_DEBUG
                tempSample += 128 * fm[chnNum].generateSample() / MAX_FM_CHANNELS;
                #elif defined PATCH_DEBUG
                tempSample += 128 * fm_patchDebug[chnNum].generateSample() / MAX_FM_CHANNELS;
                #endif
                //absolute_time_t after = get_absolute_time();
                //printf("generate fm sample: %d\n", (uint32_t)absolute_time_diff_us(before, after))
            }
            samples[i] = tempSample;
        }
        buffer->sample_count = buffer->max_sample_count;
        give_audio_buffer(ap, buffer);
        mutex_exit(&mPlayer_M);
        //printf("give audio buffer!\n");
    }
}

int main() {
    
    // attempt to overclock to 250MHz: https://www.raspberrypi.org/forums/viewtopic.php?t=304309
    // if (!set_sys_clock_khz(250000, false))
    //     printf("system clock 250MHz failed\n");
    // else
    //     printf("system clock now 250MHz\n");

    stdio_init_all();

    uint32_t owner;

    #ifdef NO_DEBUG
    //smallMidiParser sm(sampleMidi, fm);
    PlayTune mdt(sampleMidiTones4, fm);
    #endif

    mutex_init(&mPlayer_M);
    sem_init(&c1_S, 0, 2);

    multicore_launch_core1(core1_entry);

    sem_acquire_blocking(&c1_S);

    printf("acquire blocking already!\n");

    #ifdef NO_DEBUG
    mdt.startPlaying();
    #elif defined PATCH_DEBUG
    fmPatchTuner fpt;
    fm_patchDebug[0].setFrequency(440);
    #endif

    // 125MHz:
    // average 3~4 uS for one operator!
    // average 14~16 uS for one FM channel!
    // 250MHz:
    // average 7~8 uS for one FM channel!

    // RP2040 interpolator:
    // 125MHz:
    // average less than 1 uS for one operator!
    // average 4~5 uS for one FM channel!
    while (true)
    {
        #ifdef NO_DEBUG
        mdt.stepScore(&mPlayer_M);
        #elif defined SINE_DEBUG
        sleep_ms(250);
        #elif defined PATCH_DEBUG
        fpt.inputSelectLoop();
        #endif
    }
    puts("\n");
    return 0;
}
