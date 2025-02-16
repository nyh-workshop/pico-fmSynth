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

#include <LittleFS.h>
#include <MD_MIDIFile.h>

#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/structs/clocks.h"

#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "pico/sem.h"

#include "sampleMidi.h"

#include "fmSynth_picoI2sAudioDriver.h"

fmSynthPicoI2s* tPlayer = nullptr;
MD_MIDIFile SMF;

void midiCallback(midi_event *pev)
// Called by the MIDIFile library when a file event needs to be processed
// thru the midi communications interface.
// This callback is set up in the setup() function.
{
  // Define constants for MIDI channel voice message IDs
  const uint8_t NOTE_OFF = 0x80;  // note on
  const uint8_t NOTE_ON = 0x90;   // note off. NOTE_ON with velocity 0 is same as NOTE_OFF

  switch (pev->data[0]) {
    case NOTE_OFF:  // [1]=note no, [2]=velocity
      // playNote(pev->data[1], SILENT);
      tPlayer->midiNoteOff(pev->data[1]);
      Serial.printf("NOTE_OFF ch: %d\n", pev->data[1]);
      break;

    case NOTE_ON:  // [1]=note_no, [2]=velocity
      // Note ON with velocity 0 is the same as off
      // playNote(pev->data[1], (pev->data[2] == 0) ? SILENT : ACTIVE);
      // Velocity is not included for now!
      tPlayer->midiNoteOn(pev->data[1]);
      Serial.printf("NOTE_ON trk: %d, ch: %d, no: %d\n", pev->track, pev->channel, pev->data[1]);
      break;

    default:
      break;
  }
}

static enum { S_IDLE, S_PLAYING } state = S_IDLE;

void setup() {
  // uint32_t owner;
  Serial1.begin(115200);
  Serial.begin(115200);

  delay(1000);

  // Note: Please put the instrument names that exist in patch.cpp, or else it asserts!
  tPlayer = new fmSynthPicoI2s((std::string)"GUITAR");

  // Initialize LittleFS:
  if(!LittleFS.begin())
  {
    Serial.println("LittleFS init fail!");
    while(1);
  }

  // Initialize MIDIFile:
  SMF.begin(&LittleFS);
  SMF.setMidiHandler(midiCallback);

  Serial.println("Start playing song now!");

  // Load a MIDI file from there:
  int err = SMF.load("AiWaKatsu.mid");

  if (err != MD_MIDIFile::E_OK)
  {    
    Serial.printf("SMF load error: %d", err);
    while(1);
  }

  Serial.printf("Filename: %s\n", SMF.getFilename());
  Serial.printf("Format: %d\n", SMF.getFormat());

  state = S_PLAYING;
}

void loop() {
  switch (state) {
    case S_IDLE:
      delay(500);
      break;
    case S_PLAYING:
      tPlayer->playSamples();
      if (!SMF.isEOF()) {
        SMF.getNextEvent();
      }
      else
      {
        Serial.println("Playing done! Idle mode now...");
        state = S_IDLE;
      }
      break;
    default:
      break;
  }
}