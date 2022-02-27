# Raspberry Pico Simple FM synth

This is a simple DX-9 style 6-channel FM synth for the Raspberry Pico.

Requirements:
- Raspberry Pico SDK
- Pico-Extras (https://github.com/raspberrypi/pico-extras)
- I2S DAC (PCM5102)

Due to the lack of floating point unit in the Raspberry Pico, fixed points are used instead. Fixed point library: https://github.com/gbmhunter/MFixedPoint

The sampling rate is limited to 22050Hz because the resolution of the fixed point is only at 15:16.

The sounds and patches being output are not exactly DX-9 - it's only a **rough approximation** of that instrument. The envelope generator is a rudimentary ADSR state machine and it is not based on the more complicated designs of those of the other DX series. With that limited sampling rate and resolution, some of the patches may sound off if compared to listening to the actual DX-7 or DX-9 ones.

This branch uses the RP2040's interpolator module to generate the sine wave using DDS. The interpolator for now is using only one lane. The interpolator module has to be shared by 4 other operators, and it is multiplexed by saving and restoring the accumulators on each sample.

Only 4 operators are used, and each of the operator has an envelope for each FM channel. Reference for this: https://asb2m10.github.io/dexed/. A lot of CPU time are being spent on working on each of these channel.

Len Shustek's Miditones (https://github.com/LenShustek/miditones) is used in this demonstration.

There are clicking noises between note switches - this is mitigated by using an older version of the Miditones (v1.12) where there are note stops before the note change happens. This note stops allow the brief release of the note in the envelope generator and significantly minimizes the unpleasent noise.

Alongside with adding the usage of the interpolator modules, a large part of the code is restructured for increased portability to other microcontrollers or systems. Restructuring is still in progress and more suggestions are welcome! :D