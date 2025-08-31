# Raspberry Pico 1 and 2 Simple FM synth Arduino Library

This is a simple DX-9 style 6-channel FM synth library for the Raspberry Pico 1 and 2.

## Features
- DX-9 style 6-channel FM synth.
- Play files that are inside the flash using LittleFS!

## Requirements
- Arduino IDE 2.3.2 and above
- Raspberry Pi RP2040/RP2350 board library (4.1.1)
- I2S DAC (PCM5102, or MAX98357)
- Modified MajicDesigns' [MD_MIDIFile](https://github.com/nyh-workshop/MD_MIDIFile) library to play MIDI files from LittleFS (Flash size: 128KB)
- [arduino-littlefs-upload extension](https://github.com/earlephilhower/arduino-littlefs-upload)
- USB connection for UART, FM synth module debug outputs at Serial1

This branch uses the **RP2040's and RP2350's interpolator module** to generate the sine wave using DDS. The interpolator for now is using only one lane. The interpolator module has to be shared by 4 other operators, and it is multiplexed by saving and restoring the accumulators on each sample.

Only **4 operators** are used, and each of the operator has an envelope for each FM channel: [Dexed Reference](https://asb2m10.github.io/dexed/).

Using MajicDesigns' [MIDI parser](https://github.com/nyh-workshop/MD_MIDIFile) with LittleFS support instead of MidiTones. Some MIDI files might not play properly in the system and it is currently being investigated too.

## Build a circuit example (Default: `BCLK=31`, `WS=32`, `DATA=34`)
![](/media/pico-fmSynth_bb.png)

## Installation and usage instructions
- Unzip the [MIDI parser](https://github.com/nyh-workshop/MD_MIDIFile) into the Arduino's `Documents\Arduino\Libraries` folder.
- Install the [arduino-littlefs-upload extension](https://github.com/earlephilhower/arduino-littlefs-upload) into the `C:\Users\<username>\.arduinoIDE\plugins\`.
- At the `Tools->Flash Size` select `Sketch:3968KB,FS:128KB` before compiling.
- A sample midi file is there inside for testing.

### How to init library
- At the beginning of the sketch, add this:
```
#include <LittleFS.h>
#include <MD_MIDIFile.h>
#include <fmSynth_picoI2sAudioDriver.h>
```
- For default pins (`BCLK=31`, `WS=32`, `DATA=34`) just init the library as follows with the instrument of your choice (see *Supported Instrument List*):
```
fmSynthPicoI2s("GUITAR");
```
- If you need to define your own pins, here is an example:
```
fmSynthPicoI2s(29, 31, "GUITAR");
```

### How to copy the files into the flash
- Put the midi file into the sketch folder's `Data` folder. Make sure you have the same midi file name in that .ino too (SMF.load)!
- Get the Raspberry Pico to be in Boot mode.
- `Ctrl-Shift-P` in the Arduino IDE -> `Upload LittleFS to...`. Your midi files are now saved into the flash! :D
- Reset the Raspberry Pico and let the music play.

## Supported Instrument List
- BRASS1
- TBULARBELS
- MARIMBA
- E.PIANO
- BELLS
- TOYKBD
- GUITAR
- FLUTE
- PIPEORGAN2
- HARPSICH1
- JAZZGUIT2
- LUTE

## Limitations
- **RP2040**: since it does not have a floating point unit, [gbmhunter's Fixed Point library](https://github.com/gbmhunter/MFixedPoint) is used there. The sample rate is limited to 22050Hz because resolution of the fixed point is only at **15:16**.

- **RP2350 (ARM Cortex-M33)**: floating point is used and 44100Hz sample rate is used.

- **RP2350's RISC-V cores are not supported for now. Porting and testing still in progress.**

The sounds and patches being output are not exactly DX-9 - it's only a **rough approximation** of that instrument. The envelope generator is a **rudimentary ADSR state machine** and it is not based on the more complicated designs of those of the other DX series. With that limited sampling rate and resolution, some of the patches may sound off if compared to listening to the actual DX-7 or DX-9 ones.

## Volume Control
**Note**: This is still under testing, and sample values may wrap around and cause bad distortions if the volume multiplier is too high. I'm only setting `VOLUME_DEFAULT = 1`, `VOLUME_LOUD = 2`, `VOLUME_LOUDER = 4` and `VOLUME_LOUDEST = MAX_FM_CHANNELS` as a start.

Some instruments can also make distorted noise when the volume is too loud. It is recommended to scale down if that happens! :D

### Setting volume example:
```
tPlayer->setVolume(VOLUME_LOUDER);
```

## Main Updates
***Update 31-Aug-2025*** - Added primitive volume control.

***Update 16-Feb-2025*** - Arduino Library support.

***Update 13-Feb-2025*** - Added support for playing MIDI files from Flash using LittleFS and the modified MajicDesigns' MIDI parser.

***Update 08-Feb-2025*** - This is being ported to RP2350 and for Arduino platform. Sine test module added for convenience.

***Update 04-June-2023*** - A very large part of the code has been restructured - modules are now more isolated and clearly defined. The interpolator module is still coupled to the Oscillator - more plans to separate this too in the future. However, it is decided that the part where you can create and modify patches has been removed and planned to be relocated to another separate app. If you need to still create patches, you can check the following instructions. :D

## Sine test
Due to the difficulty of porting this to another architecture and/or platform, a short sine test is inserted inside. There should be only a **440Hz sine wave** being output when you add 'True' during initializing the object (use default pins only!):

```
fmSynthPicoI2s tunePlayer((bool)true);

while(1)
{
    tunePlayer.playSamples();
}
```

## Benchmarks
|Pico|Sample Rate (Hz)|Average time for one FM channel (uS)|
|----|-----------|-------------------------------|
|RP2040 @ 125MHz, no interpolator, sine generation using Bhaskara I method|22050|14~16|
|RP2040 @ 250MHz, no interpolator, sine generation using Bhaskara I method|22050|7~8|
|RP2040 @ 125MHz, interpolator |22050|4~5|
|RP2350 (ARM Cortex-M33) @ 150MHz, interpolator |44100|1~3|

The FM channel processing time can be sampled by getting the time before `fmc[chnNum].generateSample()` and after.
