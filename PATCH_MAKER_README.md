## Instructions for creating and modifying custom patches

- In `fmSynth/patch.cpp`, copy the template patch p255, like this:
```
// Patch Maker, using algorithm 7 as default:
//-----L0-----L1------L3-----R0-----R1-----R3---Sustain--Ratio---
const struct fmPatch p255 {
	"PATCHMAKER",
		7,
		0,
		1.00f, 0.20f, 0.00f, 0.01f, 0.50f, 0.00f, 0.50f, 1.00f, // osc3 here!
		0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f,
		0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f,
		0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f
};
```
- and then stick it to the end of the file. Relabel the new template 'p255' it to your own name, example 'p123'.
- In your own patch, change the "PATCHMAKER" to whatever instrument name you like, recommended within 11 characters.
- Put the 'p123' into the "fmPatchList" struct at the end of the file:
```
const struct fmPatch fmPatchList[32] = {p00, p02, p04, p05, p07, p07_1, p09, p16, p16_1, p13, p19, p23, p27, p123, p255}; // place p123 behind p255!
```
- Compile and load it into the Raspberry Pico.
- Using your favourite terminal software, set it to 115200 baud at `Serial1`, and a table of the envelope values and ratio is presented, example:
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
|w|change frequency 440Hz or 880Hz
|h|display the help|
|+|increase value by 0.01s|
|-|decrease value by 0.01s|
|q|select oscillator|
|a|select algorithm|
|f|select feedback|
|z|select adsr attack rate (R0)|
|x|select adsr decay rate (R1)|
|c|select adsr release rate (R3)|
|v|select adsr L3|
|b|select adsr L1|
|n|select adsr sustain|
|m|select adsr L0|
|l|select oscillator ratio|
|space|play 440hz note|

![image](https://user-images.githubusercontent.com/20377029/160284397-d426b415-ee33-42ad-8d99-226f3c28336b.png)

(apps.diagrams.net)

**Note: the units of R0, R1, R3 and Sustain are in seconds!**

- Once you done tuning it, screenshot the table and then modify the numbers accordingly in that newly created patch.
- Finally, enjoy the new patch!
