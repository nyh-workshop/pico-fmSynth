#ifndef _FMPATCHTUNER_H
#define _FMPATCHTUNER_H

#include "fmSynth_main.h"
#include "fmChannel.h"

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

class fmPatchTuner {
    public:
    void inputSelectLoop();
    void processInputSelection(char selection);
    private:
    PatchSelectState selectPatch = ADSR_R0;
    PatchAdjustState adjustPatch = MS_10_SELECT;
    uint8_t oscSelect = 0;
};

#endif