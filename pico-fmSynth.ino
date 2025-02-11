// pico-fmSynth - ported to Arduino.
// Branch: interp_oneCore

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
    Serial.println("Entering core1! :D\n");
    uint32_t owner;

    fmSynthPicoI2s tunePlayer(false);

    tunePlayer.playScore(mdtFile1);

    struct repeating_timer timer;
    add_repeating_timer_ms(-1, repeating_timer_callback, &tunePlayer, &timer);

    // RP2040's interpolator module needs to be configured at the respective core before running.
    // configureInterpLanes();

    sem_release(&c1_S);

    Serial.println("releasing semaphore now!\n");

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

// fmSynthPicoI2s* fmSynth;

void setup() {
  // uint32_t owner;
  Serial1.begin(115200);
  Serial.begin(115200);

  delay(1000);

  // fmSynth = new fmSynthPicoI2s(true);

  mutex_init(&mPlayer_M);
  sem_init(&c1_S, 0, 2);

  multicore_launch_core1(core1_entry);

  sem_acquire_blocking(&c1_S);

  Serial.println("Core1 now playing tune!");
  
  Serial.println("Setup done!");
}

void loop() {
  // Without interpolator (22050Hz), oscillator using Bhaskara I method:
  // 125MHz:
  // average 3~4 uS for one operator!
  // average 14~16 uS for one FM channel!
  // 250MHz:
  // average 7~8 uS for one FM channel!

  // RP2040 interpolator (22050Hz):
  // 125MHz:
  // average less than 1 uS for one operator!
  // average 4~5 uS for one FM channel!

  // RP2350 interpolator (44100Hz), with floating point:
  // 150MHz:
  // average 1~3us for one FM channel!
  delay(500);
}