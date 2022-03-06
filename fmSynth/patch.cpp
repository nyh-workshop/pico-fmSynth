#include "patch.h"

// Testing patch only, osc0 outputs sine wave and a short envelope:
//-----L0-----L1------L3-----R0-----R1-----R3---Sustain--Ratio---
const struct fmPatch p00 {
	"TEST00",
		0,
		0,
		1.00f, 0.20f, 0.00f, 0.01f, 0.50f, 0.00f, 5.00f, 1.00f,
		0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f,
		0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f,
		0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f
};

// DX7 default - Brass 1 (loosely based on rom1a.syx):
//-----L0-----L1------L3-----R0-----R1-----R3---Sustain--Ratio---
const struct fmPatch p02 {
	"BRASS1",
		6,
		0,
		0.80f, 0.70f, 0.01f, 0.10f, 0.05f, 0.05f, 5.00f, 1.00f,
		0.80f, 0.70f, 0.01f, 0.10f, 0.05f, 0.05f, 5.00f, 1.00f,
		0.80f, 0.70f, 0.01f, 0.10f, 0.05f, 0.05f, 5.00f, 1.00f,
		0.80f, 0.70f, 0.01f, 0.10f, 0.05f, 0.05f, 5.00f, 0.99f
};

// DX9 set 4 - Tubular Bells:
//-----L0-----L1------L3-----R0-----R1-----R3---Sustain--Ratio---
const struct fmPatch p04 {
	"TBULARBELS",
		5,
		5,
		0.70f, 0.05f, 0.01f, 0.01f, 0.70f, 4.25f, 0.00f, 1.00f,
		0.70f, 0.05f, 0.01f, 0.01f, 8.00f, 4.25f, 0.00f, 3.51f,
		0.70f, 0.05f, 0.01f, 0.01f, 0.70f, 4.25f, 0.00f, 1.00f,
		0.70f, 0.05f, 0.01f, 0.01f, 8.00f, 4.25f, 0.00f, 3.51f
};

// DX9 set 4 - Marimba:
//-----L0-----L1------L3-----R0-----R1-----R3---Sustain--Ratio---
const struct fmPatch p05 {
	"MARIMBA",
		5,
		3,
		0.60f, 0.35f, 0.01f, 0.01f, 0.10f, 1.25f, 0.00f, 1.00f,
		0.60f, 0.35f, 0.01f, 0.01f, 0.10f, 1.25f, 0.00f, 5.02f,
		0.60f, 0.35f, 0.01f, 0.01f, 0.10f, 1.25f, 0.00f, 1.00f,
		0.60f, 0.05f, 0.01f, 0.01f, 0.10f, 1.25f, 0.00f, 7.02f
};

// DX9 master set - E.Piano:
//-----L0-----L1------L3-----R0-----R1-----R3---Sustain--Ratio---
const struct fmPatch p07 {
	"E.PIANO",
		5,
		6,
		1.00f, 0.05f, 0.01f, 0.03f, 3.20f, 0.25f, 0.00f, 1.00f,
		1.00f, 0.05f, 0.01f, 0.03f, 2.30f, 0.25f, 0.00f, 1.00f,
		1.00f, 0.05f, 0.01f, 0.03f, 2.30f, 0.25f, 0.00f, 1.00f,
		0.10f, 0.02f, 0.01f, 0.03f, 0.37f, 0.125f, 0.00f, 10.40f
};

// DX9 set 4 - Bells:
//-----L0-----L1------L3-----R0-----R1-----R3---Sustain--Ratio---
const struct fmPatch p07_1 {
	"BELLS",
		5,
		6,
		0.70f, 0.05f, 0.01f, 0.01f, 5.00f, 4.25f, 0.00f, 1.03f,
		0.70f, 0.05f, 0.01f, 0.01f, 5.00f, 4.25f, 0.00f, 1.27f,
		0.70f, 0.05f, 0.01f, 0.01f, 5.00f, 4.25f, 0.00f, 0.50f,
		0.70f, 0.05f, 0.01f, 0.01f, 7.00f, 4.25f, 0.00f, 1.74f
};

// DX9 set 2 - Toy Keyboard:
//-----L0-----L1------L3-----R0-----R1-----R3---Sustain--Ratio---
const struct fmPatch p09 {
	"TOYKBD",
		7,
		2,
		0.60f, 0.05f, 0.01f, 0.01f, 2.33f, 0.33f, 0.00f, 1.00f,
		0.60f, 0.05f, 0.01f, 0.01f, 0.66f, 0.33f, 0.00f, 2.00f,
		1.00f, 0.05f, 0.01f, 0.01f, 0.66f, 0.66f, 0.00f, 4.44f,
		0.60f, 0.05f, 0.01f, 0.01f, 2.33f, 0.66f, 0.00f, 12.69f
};

// DX9 set 4 - Guitar 3:
//-----L0-----L1------L3-----R0-----R1-----R3---Sustain--Ratio---
const struct fmPatch p16 {
	"GUITAR3",
		3,
		2,
		0.50f, 0.05f, 0.01f, 0.01f, 2.00f, 0.75f, 0.00f, 1.00f,
		0.50f, 0.05f, 0.01f, 0.01f, 2.00f, 0.75f, 0.00f, 3.00f,
		0.50f, 0.05f, 0.01f, 0.01f, 2.00f, 0.75f, 0.00f, 1.00f,
		0.25f, 0.05f, 0.01f, 0.01f, 1.00f, 0.75f, 0.00f, 5.00f
};

// DX9 set 2 - Pipe Organ 2:
//-----L0-----L1------L3-----R0-----R1-----R3---Sustain--Ratio---
const struct fmPatch p13 {
	"PIPEORGAN2",
		5,
		7,
		0.70f, 0.65f, 0.01f, 0.05f, 2.00f, 0.75f, 100.00f, 1.00f,
		0.70f, 0.65f, 0.01f, 0.05f, 2.00f, 0.75f, 100.00f, 8.00f,
		0.70f, 0.65f, 0.01f, 0.05f, 2.00f, 0.75f, 100.00f, 1.00f,
		0.70f, 0.65f, 0.01f, 0.05f, 1.00f, 0.75f, 100.00f, 0.50f
};

// DX7 default - Harpsichord 1 (loosely based on rom3b.syx):
//-----L0-----L1------L3-----R0-----R1-----R3---Sustain--Ratio---
const struct fmPatch p19 {
		"HARPSICH1",
		5,
		5,
		0.90f, 0.15f, 0.00f, 0.01f, 0.50f, 0.15f, 0.00f, 1.01f,
		0.90f, 0.15f, 0.00f, 0.01f, 0.50f, 0.01f, 1.00f, 4.00f,
		0.90f, 0.15f, 0.00f, 0.01f, 0.50f, 0.15f, 0.00f, 0.99f,
		0.90f, 0.15f, 0.00f, 0.01f, 0.50f, 0.01f, 1.00f, 5.00f
};

// DX7 default - Jazz Guitar 2 (loosely based on rom3b.syx):
//-----L0-----L1------L3-----R0-----R1-----R3---Sustain--Ratio---
const struct fmPatch p23 {
		"JAZZGUIT2",
		3,
		3,
		0.90f, 0.15f, 0.00f, 0.01f, 0.50f, 0.55f, 0.00f, 1.00f,
		0.90f, 0.15f, 0.00f, 0.01f, 0.50f, 0.35f, 0.00f, 3.00f,
		0.90f, 0.15f, 0.00f, 0.01f, 0.50f, 0.55f, 0.00f, 14.00f,
		0.90f, 0.75f, 0.30f, 0.95f, 0.01f, 0.01f, 1.00f, 3.44f
};

// DX7 default - Lute (loosely based on rom3b.syx):
//-----L0-----L1------L3-----R0-----R1-----R3---Sustain--Ratio---
const struct fmPatch p27 {
		"LUTE",
		2,
		0,
		0.90f, 0.15f, 0.00f, 0.01f, 1.00f, 0.55f, 0.00f, 1.00f,
		0.90f, 0.15f, 0.00f, 0.01f, 1.00f, 0.35f, 0.00f, 2.01f,
		0.90f, 0.15f, 0.00f, 0.01f, 1.00f, 0.55f, 0.00f, 4.01f,
		0.90f, 0.30f, 0.10f, 0.01f, 0.05f, 0.01f, 1.00f, 5.95f
};

const struct fmPatch fmPatchList[32] = {p00, p02, p04, p05, p07, p07_1, p09, p16, p13, p19, p23, p27};