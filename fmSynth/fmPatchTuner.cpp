#include "fmPatchTuner.h"

extern fmChannel fm_patchDebug[1];

extern const float MAX_L0;

void fmPatchTuner::inputSelectLoop() {
    char selection = ' ';
    printf("pico-fmSynth patch debug:\n");
    fm_patchDebug[0].printChannelDetails();
    printf("> ");
    selection = uart_getc(uart0);
    printf("%c\n", selection);
    processInputSelection(selection);
}

void fmPatchTuner::processInputSelection(char selection) {
    // not implemented yet:
    // 1ms select = 'p'
    // 10ms select = '['
    // 100ms select = ']'
    // 10000ms select = '\'

    // implemented:
    // increase value = '+'
    // decrease value = '-'
    // select oscillator = 'q'
    // select algorithm = 'a'
    // select feedback = 'f'
    // select adsr attack rate (R0) = 'z'
    // select adsr decay rate (R1) = 'x'
    // select adsr release rate (R3) = 'c'
    // select adsr L0 = 'v'
    // select adsr L1 = 'b'
    // select adsr sustain = 'n'
    // select adsr L3 = 'm'
    // select oscillator ratio = 'l'
    // hit note = ' '
    //
    switch(selection) {
        case '+':
        {
            float array[8];
            fm_patchDebug[0].getOscDetails(oscSelect, array);
            if (selectPatch == ADSR_L0) {
                float L0 = array[ADSR_L0];
                L0 += 0.01;
                (L0 >= MAX_L0) ? L0 = MAX_L0 : L0;
                array[ADSR_L0] = L0;
                fm_patchDebug[0].setOscDetails(oscSelect, array);
            }
            if (selectPatch == ADSR_L1) {
                float L1 = array[ADSR_L1];
                L1 += 0.01;
                (L1 >= MAX_L1) ? L1 = MAX_L1 : L1;
                array[ADSR_L1] = L1;
                fm_patchDebug[0].setOscDetails(oscSelect, array);
            }
            if (selectPatch == ADSR_L3) {
                float L3 = array[ADSR_L3];
                L3 += 0.01;
                (L3 >= MAX_L3) ? L3 = MAX_L3 : L3;
                array[ADSR_L3] = L3;
                fm_patchDebug[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == ADSR_R0) {
                float R0 = array[ADSR_R0];
                R0 += 0.01;
                (R0 >= MAX_R0) ? R0 = MAX_R0 : R0;
                array[ADSR_R0] = R0;
                fm_patchDebug[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == ADSR_R1) {
                float R1 = array[ADSR_R1];
                R1 += 0.01;
                (R1 >= MAX_R1) ? R1 = MAX_R1 : R1;
                array[ADSR_R1] = R1;
                fm_patchDebug[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == ADSR_R3) {
                float R3 = array[ADSR_R3];
                R3 += 0.01;
                (R3 >= MAX_R3) ? R3 = MAX_R3 : R3;
                array[ADSR_R3] = R3;
                fm_patchDebug[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == ADSR_SUSTAIN) {
                float sustain = array[ADSR_SUSTAIN];
                sustain += 0.01;
                (sustain >= MAX_SUSTAIN) ? sustain = MAX_SUSTAIN : sustain;
                array[ADSR_SUSTAIN] = sustain;
                fm_patchDebug[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == PATCH_RATIO) {
                float ratio = array[PATCH_RATIO];
                ratio += 0.01;
                (ratio >= MAX_RATIO) ? ratio = MAX_RATIO : ratio;
                array[PATCH_RATIO] = ratio;
                fm_patchDebug[0].setOscDetails(oscSelect, array);
            }
            break;
        }
        case '-':
        {   
            float array[8];
            fm_patchDebug[0].getOscDetails(oscSelect, array);
            if (selectPatch == ADSR_L0) {
                float L0 = array[ADSR_L0];
                L0 -= 0.01;
                (L0 <= 0.00) ? L0 = 0.00 : L0;
                array[ADSR_L0] = L0;
                fm_patchDebug[0].setOscDetails(oscSelect, array);
            }
            if (selectPatch == ADSR_L1) {
                float L1 = array[ADSR_L1];
                L1 -= 0.01;
                (L1 <= 0.00) ? L1 = 0.00 : L1;
                array[ADSR_L1] = L1;
                fm_patchDebug[0].setOscDetails(oscSelect, array);
            }
            if (selectPatch == ADSR_L3) {
                float L3 = array[ADSR_L3];
                L3 -= 0.01;
                (L3 <= 0.00) ? L3 = 0.00 : L3;
                array[ADSR_L3] = L3;
                fm_patchDebug[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == ADSR_R0) {
                float R0 = array[ADSR_R0];
                R0 -= 0.01;
                (R0 <= 0.01) ? R0 = 0.01 : R0;
                array[ADSR_R0] = R0;
                fm_patchDebug[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == ADSR_R1) {
                float R1 = array[ADSR_R1];
                R1 -= 0.01;
                (R1 <= 0.01) ? R1 = 0.01 : R1;
                array[ADSR_R1] = R1;
                fm_patchDebug[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == ADSR_R3) {
                float R3 = array[ADSR_R3];
                R3 -= 0.01;
                (R3 <= 0.01) ? R3 = 0.01 : R3;
                array[ADSR_R3] = R3;
                fm_patchDebug[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == ADSR_SUSTAIN) {
                float sustain = array[ADSR_SUSTAIN];
                sustain -= 0.01;
                (sustain <= 0.00) ? sustain = 0.00 : sustain;
                array[ADSR_SUSTAIN] = sustain;
                fm_patchDebug[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == PATCH_RATIO) {
                float ratio = array[PATCH_RATIO];
                ratio -= 0.01;
                (ratio <= 0.00) ? ratio = 0.00 : ratio;
                array[PATCH_RATIO] = ratio;
                fm_patchDebug[0].setOscDetails(oscSelect, array);
            }
            break;
        }
        case '[':
            printf("\n10ms selected!\n");
            adjustPatch = MS_10_SELECT;
            break;
        case 'v':
            printf("\nADSR L3 selected!\n");
            selectPatch = ADSR_L3;
            break;
        case 'b':
            printf("\nADSR L1 selected!\n");
            selectPatch = ADSR_L1;
            break;
        case 'm':
            printf("\nADSR L0 selected!\n");
            selectPatch = ADSR_L0;
            break;
        case 'n':
            printf("\nADSR sustain selected!\n");
            selectPatch = ADSR_SUSTAIN;
            break;
        case 'c':
            printf("\nADSR release rate (R3) selected!\n");
            selectPatch = ADSR_R3;
            break;
        case 'x':
            printf("\nADSR decay rate (R1) selected!\n");
            selectPatch = ADSR_R1;
            break;
        case 'z':
            printf("\nADSR attack rate (R0) selected!\n");
            selectPatch = ADSR_R0;
            break;
        case 'f':
        {
            uint8_t fbShift = 0;
            printf("\nselect feedback shift value (0-7): ");
            fbShift = uart_getc(uart0) - 0x30;
            if (fbShift > 7) {
                fbShift = 7;
                printf("\nMaximum feedback shift - setting to 7!\n");
            }
            fm_patchDebug[0].setFeedback(fbShift);
            break;
        }
        case 'a':
        {
            uint8_t algorithm = 0;
            printf("\nselect algorithm: ");
            algorithm = uart_getc(uart0) - 0x30;
            if (algorithm > 8)
            {
                printf("\ninvalid algorithm! 0 to 8 only!\n");
                break;
            }
            else
            {
                fm_patchDebug[0].selectAlgorithm(algorithm);
                printf("\nAlgorithm %d selected!\n", algorithm);
                break;
            }
        }
        case 'q':
        {
            uint8_t oscillator = 0;
            printf("\nselect oscillator: ");
            oscillator = uart_getc(uart0) - 0x30;
            if (oscillator > 3)
            {
                printf("\noscillators 0-3 only!\n");
                break;
            } 
            else
            {
                printf("\noscillator %d selected!\n", oscillator);
                oscSelect = oscillator;
                break;
            }
        }
        case 'l':
            printf("\noscillator ratio selected!\n");
            selectPatch = PATCH_RATIO;
            break;
        case ' ':
            fm_patchDebug[0].noteOn();
            break;
        default:
            printf("Invalid selection!\n");
            break;
    }
}