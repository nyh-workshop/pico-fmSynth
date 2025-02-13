# Raspberry Pico 2 Simple FM synth (Prototype Arduino Branch for RP2350)

This is a simple DX-9 style 6-channel FM synth for the Raspberry Pico 2.

Requirements:
- Arduino IDE 2.3.2 and above
- Raspberry Pi RP2040/RP2350 board library (4.1.1)
- I2S DAC (PCM5102)
- Modified MajicDesigns' [MD_MIDIFile](https://github.com/nyh-workshop/MD_MIDIFile) library to play MIDI files from LittleFS (Flash size: 128KB)
- [arduino-littlefs-upload extension](https://github.com/earlephilhower/arduino-littlefs-upload)
- USB connection for UART, FM synth module debug outputs at Serial1

Floating point is used for this Raspberry Pico 2 with ARM Cortex-M33 cores.

**RISC-V cores are not supported for now. This will be done when porting is fully complete.**

The sounds and patches being output are not exactly DX-9 - it's only a **rough approximation** of that instrument. The envelope generator is a rudimentary ADSR state machine and it is not based on the more complicated designs of those of the other DX series. With that limited sampling rate and resolution, some of the patches may sound off if compared to listening to the actual DX-7 or DX-9 ones.

This branch uses the **RP2350's interpolator module** to generate the sine wave using DDS. The interpolator for now is using only one lane. The interpolator module has to be shared by 4 other operators, and it is multiplexed by saving and restoring the accumulators on each sample.

Only **4 operators** are used, and each of the operator has an envelope for each FM channel: [Dexed Reference](https://asb2m10.github.io/dexed/). Currently, it is using 1~3uS to generate one sample on the channel.

Using MajicDesigns' [MIDI parser](https://github.com/nyh-workshop/MD_MIDIFile) with LittleFS support instead of MidiTones. Currently figuring out how to display and check MIDI info before playing the MIDI file. Some MIDI files might not play properly in the system and it is currently being investigated too.

## Installation and usage instructions
- Unzip the [MIDI parser](https://github.com/nyh-workshop/MD_MIDIFile) into the Arduino's `Documents\Arduino\Libraries` folder.
- Install the [arduino-littlefs-upload extension](https://github.com/earlephilhower/arduino-littlefs-upload) into the `C:\Users\<username>\.arduinoIDE\plugins\`.
- At the `Tools->Flash Size` select `Sketch:3968KB,FS:128KB` before compiling.
- A sample midi file is there inside for testing.

### How to copy the files into the flash
- Put the midi file into the sketch folder's `Data` folder. Make sure you have the same midi file name in that .ino too (SMF.load)!
- Get the Raspberry Pico to be in Boot mode.
- `Ctrl-Shift-P` in the Arduino IDE -> `Upload LittleFS to...`. Your midi files are now saved into the flash! :D
- Reset the Raspberry Pico and let the music play.

## Main Updates
***Update 13-Feb-2025*** - Added support for playing MIDI files from Flash using LittleFS and the modified MajicDesigns' MIDI parser.

***Update 08-Feb-2025*** - This is being ported to RP2350 and for Arduino platform. Sine test module added for convenience.

***Update 04-June-2023*** - A very large part of the code has been restructured - modules are now more isolated and clearly defined. The interpolator module is still coupled to the Oscillator - more plans to separate this too in the future. However, it is decided that the part where you can create and modify patches has been removed and planned to be relocated to another separate app. If you need to still create patches, you can check the following instructions. :D

## Sine test
Due to the difficulty of porting this to another architecture and/or platform, a short sine test is inserted inside. There should be only a 440Hz sine wave being output when you add 'True' when you init the object in that way:

```
fmSynthPicoI2s tunePlayer(true);

while(1)
{
    tunePlayer.playSamples();
}
```

## Future expansions
More complete MIDI file checking routines.

## Instructions for creating and modifying custom patches (use commit [4b1e62](https://github.com/nyh-workshop/pico-fmSynth/commit/4b1e622bf7494a5b7b671c2d291cbe83a93ac167)) :

- Comment out the define NO_DEBUG and then uncomment the PATCH_DEBUG inside fmSynth_main.h.
- Afterwards in fmSynth/patch.cpp, copy the template patch p255, like this:
```
// Debugging patch only, using algorithm 7 as default:
//-----L0-----L1------L3-----R0-----R1-----R3---Sustain--Ratio---
const struct fmPatch p255 {
    "TEST01",
    7,
    0,
    1.00f, 0.20f, 0.00f, 0.01f, 0.50f, 0.00f, 0.50f, 1.00f, // osc3 here!
    0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f,
    0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f,
    0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f		
};
```
- and then stick it to the end of the file. Relabel the new template 'p255' it to your own name, example 'p123'.
- Change the "Test01" to whatever instrument name you like, recommended within 11 characters.
- Put the 'p123' into the "fmPatchList" struct at the end of the file:
```
const struct fmPatch fmPatchList[32] = {p00, p02, p04, p05, p07, p07_1, p09, p16, p16_1, p13, p19, p23, p27, p123, p255}; // place p123 behind p255!
```
- Compile and load it into the RP2040.
- Using your favourite terminal software, set it to 115200 baud, and a table of the envelope values and ratio is presented, example:
```
pico-fmSynth patch debug:
a: 4            f: 3
o--L0---L1---L3---R0---R1---R3----S----Ratio
0: 0.50 0.45 0.00 0.03 0.15 0.75  3.00 1.00
1: 0.50 0.05 0.00 0.03 2.00 0.75  0.00 0.99
2: 0.50 0.05 0.00 0.03 2.00 0.75  0.00 2.00
3: 0.50 0.25 0.00 0.03 0.25 0.75  3.00 1.01
```
- Select the oscillator first by pressing 'q', then 0 to 3.
- Refer to the following table to help you set the ADSR values and the ratio:

|key|function|
|---|---|
|+|increase value by 0.01s|
|-|decrease value by 0.01s|
|q|select oscillator|
|a|select algorithm|
|f|select feedback|
|z|select adsr attack rate (R0)|
|x|select adsr decay rate (R1)|
|c|select adsr release rate (R3)|
|v|select adsr L0|
|b|select adsr L1|
|n|select adsr sustain|
|m|select adsr L3|
|l|select oscillator ratio|
|space|play 440hz note|

![image](https://user-images.githubusercontent.com/20377029/160284397-d426b415-ee33-42ad-8d99-226f3c28336b.png)

(apps.diagrams.net)

**Note: the units of R0, R1, R3 and Sustain are in seconds!**

- Once you done tuning it, screenshot the table and then modify the numbers accordingly in that newly created patch.
- Finally, enjoy the new patch!
