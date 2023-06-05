# Raspberry Pico Simple FM synth

This is a simple DX-9 style 6-channel FM synth for the Raspberry Pico.

Requirements:
- Raspberry Pico SDK
- Pico-Extras (https://github.com/raspberrypi/pico-extras)
- I2S DAC (PCM5102)
- USB connection for UART

Due to the lack of floating point unit in the Raspberry Pico, fixed points are used instead. Fixed point library: [MFixedPoint](https://github.com/gbmhunter/MFixedPoint)

The sampling rate is limited to 22050Hz because the resolution of the fixed point is only at 15:16.

The sounds and patches being output are not exactly DX-9 - it's only a **rough approximation** of that instrument. The envelope generator is a rudimentary ADSR state machine and it is not based on the more complicated designs of those of the other DX series. With that limited sampling rate and resolution, some of the patches may sound off if compared to listening to the actual DX-7 or DX-9 ones.

This branch uses the **RP2040's interpolator module** to generate the sine wave using DDS. The interpolator for now is using only one lane. The interpolator module has to be shared by 4 other operators, and it is multiplexed by saving and restoring the accumulators on each sample.

Only **4 operators** are used, and each of the operator has an envelope for each FM channel. [Reference](https://asb2m10.github.io/dexed/). A lot of CPU time are being spent on working on each of these channel.

Len Shustek's [Miditones](https://github.com/LenShustek/miditones) is used in this demonstration. For this branch (interp_oneCore) a single core is used and the timer interrupt is to track the score rather than having another core to wait for it.

There are clicking noises between note switches - this is mitigated by using an older version of the Miditones (v1.12) where there are note stops before the note change happens. This note stops allow the brief release of the note in the envelope generator and significantly minimizes the unpleasent noise.

***Update 04-June-2023*** - A very large part of the code has been restructured - modules are now more isolated and clearly defined. The interpolator module is still coupled to the Oscillator - more plans to separate this too in the future. However, it is decided that the part where you can create and modify patches has been removed and planned to be relocated to another separate app. If you need to still create patches, you can check the following instructions. :D

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
