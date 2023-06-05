/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <math.h>
#include <tusb.h>

#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/structs/clocks.h"

#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "pico/sem.h"

#include "sampleMidi.h"

#include "fmSynth_picoI2sAudioDriver.h"

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

static mutex_t mPlayer_M;
static semaphore_t c1_S;

volatile uint32_t durationMs = 0;
bool repeating_timer_callback(struct repeating_timer *t) {
    if(durationMs >= ((fmSynthPicoI2s*)t->user_data)->getNoteDurationMs())
    {
        durationMs = 0;
        ((fmSynthPicoI2s*)t->user_data)->stepScore();
    }
    else
        durationMs++;
    return true;
}

void core1_entry()
{
    printf("Entering core1! :D\n");
    uint32_t owner;

    fmSynthPicoI2s tunePlayer;

    tunePlayer.playScore(sampleMidiTones);

    struct repeating_timer timer;
    add_repeating_timer_ms(-1, repeating_timer_callback, &tunePlayer, &timer);

    // RP2040's interpolator module needs to be configured at the respective core before running.
    configureInterpLanes();

    sem_release(&c1_S);

    printf("releasing semaphore now!\n");

    while (1)
    {
        if(tunePlayer.isPlaying())
            tunePlayer.playSamples();
        else
        {
            static bool callTimerCancelledOnce;
            if(!callTimerCancelledOnce)
            {
                callTimerCancelledOnce = true;
                cancel_repeating_timer(&timer);
            }
            sleep_ms(500);
        }
    }
}

int main() {
    
    // attempt to overclock to 250MHz: https://www.raspberrypi.org/forums/viewtopic.php?t=304309
    // if (!set_sys_clock_khz(250000, false))
    //     printf("system clock 250MHz failed\n");
    // else
    //     printf("system clock now 250MHz\n");

    stdio_init_all();

    stdio_filter_driver(&stdio_usb);
    while (!tud_cdc_connected())
        sleep_ms(100);
    printf("\nUSB Serial connected!\n");

    uint32_t owner;

    mutex_init(&mPlayer_M);
    sem_init(&c1_S, 0, 2);

    multicore_launch_core1(core1_entry);

    sem_acquire_blocking(&c1_S);

    printf("Core1 now playing tune!\n");

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
        sleep_ms(500);
    }
    puts("\n");
    return 0;
}
