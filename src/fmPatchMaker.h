#ifndef _FMPATCHMAKER_H
#define _FMPATCHMAKER_H

enum PatchSelectState { ADSR_L0 = 0, ADSR_L1, ADSR_L3, ADSR_R0, ADSR_R1, ADSR_R3, ADSR_SUSTAIN, PATCH_RATIO };
enum PatchAdjustState { MS_1_SELECT = 0, MS_10_SELECT, MS_100_SELECT, MS_1000_SELECT };

constexpr float MAX_L0 = 5.0f;
constexpr float MAX_L1 = 1.0f;
constexpr float MAX_L3 = 1.0f;
constexpr float MAX_R0 = 1.0f;
constexpr float MAX_R1 = 10.0f;
constexpr float MAX_R3 = 10.0f;
constexpr float MAX_SUSTAIN = 256.00f;
constexpr float MAX_RATIO = 50.0f;

#endif