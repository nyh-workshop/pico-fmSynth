#include "fmSynth_picoI2sAudioDriver.h"

void fmSynthPicoI2s::configureInterpLanes()
{
  interp_config interpCfg = interp_default_config();

  // Lane 0 settings:
  interp_config_set_add_raw(&interpCfg, true);
  interp_config_set_cross_input(&interpCfg, 0);
  interp_config_set_mask(&interpCfg, 22, 31);
  interp_config_set_shift(&interpCfg, 0);
  interp_config_set_signed(&interpCfg, false);
  interp_set_config(interp1, 0, &interpCfg);
  // Lane 1 settings:
  interp_config_set_add_raw(&interpCfg, true);
  interp_config_set_cross_input(&interpCfg, 0);
  interp_config_set_mask(&interpCfg, 0, 9);
  interp_config_set_shift(&interpCfg, 22);
  interp_config_set_signed(&interpCfg, false);
  interp_set_config(interp1, 1, &interpCfg);
}

fmSynthPicoI2s::fmSynthPicoI2s(uint8_t aMode) {
  Serial1.println("Configure interpolator lanes...");
  configureInterpLanes();
  Serial1.println("Start up fmSynth I2S...");
  switch (aMode)
  {
  case MIDI_PLAYER:
    Serial1.println("Start up default fmSynth MIDI player!");
    for (uint8_t i = 0; i < MAX_FM_CHANNELS; i++)
    {
      fmc[i].setChannelInstrument("GUITAR");
    }
    break;
  case SINE_440HZ_TEST:
  {
    Serial1.println("Start up fmSynth I2S Sine Test mode...");
    for (uint8_t i = 0; i < MAX_FM_CHANNELS; i++)
    {
      fmc[i].setChannelInstrument("TEST00");
    }

    // Get samples for the sine test (440Hz)!
    Serial1.println("Playing 440Hz sine here...");
    while (1)
    {
      playSamples();
    }
  }
  break;
  case PATCH_MAKER:
  {
    int incomingByte = 0;
    Serial1.println("*** Patch Maker ***");
    Serial1.println("Press 'h' for Help!\n");
    setVolume(VOLUME_LOUDEST);
    for (uint8_t i = 0; i < MAX_FM_CHANNELS; i++)
    {
      fmc[i].setChannelInstrument("PATCHMAKER");
      fmc[i].setFrequency(440.0);
    }
    Serial1.print("> ");
    while (1)
    {
      if (Serial1.available() > 0)
      {
        incomingByte = Serial1.read();
        Serial1.printf("%c\n", (char)incomingByte);
        PatchMakerProcessInputSelection((char)incomingByte);
        Serial1.print("> ");
      }
      playSamples();
    }
  }
  break;
  default:
    Serial1.println("Invalid mode! Resetting back to Midi Player!");
    for (uint8_t i = 0; i < MAX_FM_CHANNELS; i++)
    {
      fmc[i].setChannelInstrument("GUITAR");
    }
    mode = MIDI_PLAYER;
    break;
  }
}

fmSynthPicoI2s::fmSynthPicoI2s(std::string aInstrument) {
  Serial1.println("Configure interpolator lanes...");
  configureInterpLanes();
  Serial1.printf("Start up fmSynth I2S with default pins and instrument: %s\n", aInstrument.c_str());
  for (uint8_t i = 0; i < MAX_FM_CHANNELS; i++)
    fmc[i].setChannelInstrument(aInstrument);
}

fmSynthPicoI2s::~fmSynthPicoI2s() {
  Serial1.println("closing fmSynthPicoI2s...");
}

// Both these midiNoteOn and midiNoteOff are based on the ESP8266Audio's method of parsing the midi notes.
void fmSynthPicoI2s::midiNoteOn(uint8_t num) {
  // Take the next channel which is not playing:
  for (uint8_t i = 0; i < MAX_FM_CHANNELS; i++) {
    if (!fmc[i].getIsPlaying()) {
      fmc[i].setIsPlaying();
      fmc[i].setMidiNoteNum(num);
      fmc[i].setFrequency(convertMidiNoteToFreq(fmc[i].getMidiNoteNum()));
      fmc[i].noteOn();
      break;
    }
  }
}

void fmSynthPicoI2s::midiNoteOff(uint8_t num) {
  // Note off the channel which is playing:
  for (uint8_t i = 0; i < MAX_FM_CHANNELS; i++) {
    if(fmc[i].getIsPlaying() && (fmc[i].getMidiNoteNum() == num))
    {
      fmc[i].clearIsPlaying();
      fmc[i].noteOff();
    }
  }  
}

int16_t fmSynthPicoI2s::getSampleTest() {
  int16_t tempSample = (int16_t)0;
  // Only one channel for sine test!
  tempSample = 128 * fmc[0].generateSample();
  return tempSample;
}

int16_t fmSynthPicoI2s::getSample() {
  int16_t tempSample = (int16_t)0;
  for (uint8_t chnNum = 0; chnNum < MAX_FM_CHANNELS; chnNum++) {
    // absolute_time_t before = get_absolute_time();
    tempSample += 128 * fmc[chnNum].generateSample() / MAX_FM_CHANNELS;
    // absolute_time_t after = get_absolute_time();
    // Serial1.printf("generate fm sample time: %d\n", (uint32_t)absolute_time_diff_us(before, after));
  }

  // Some primitive volume boost (Experimental):
  // Clamping of values will be implemented in next update. 
  tempSample *= volume;
  return tempSample;
}

// Patch Maker Functions:
void fmSynthPicoI2s::PatchMakerPrintHelp() {
  Serial1.printf("*** Patch Maker Help ***\n");
  Serial1.printf("Note: ADSR Rates are in Seconds!\n");
  Serial1.printf("+ -> Increase Value\n");
  Serial1.printf("- -> Decrease Value\n");
  Serial1.printf("q -> Select Oscillator\n");
  Serial1.printf("a -> Select Algorithm\n");
  Serial1.printf("f -> Select Feedback\n");
  Serial1.printf("z -> Select ADSR Attack Rate (R0)\n");
  Serial1.printf("x -> Select ADSR Decay Rate (R1)\n");
  Serial1.printf("c -> Select ADSR Release Rate (R3)\n");
  Serial1.printf("v -> Select ADSR L3\n");
  Serial1.printf("b -> Select ADSR L1\n");
  Serial1.printf("n -> Select ADSR Sustain\n");
  Serial1.printf("m -> Select ADSR L0\n");
  Serial1.printf("l -> Select Oscillator Ratio\n");
  Serial1.printf("Spacebar -> Play note\n");
}

void fmSynthPicoI2s::PatchMakerProcessInputSelection(char selection) {

    static PatchSelectState selectPatch = ADSR_R0;
    static PatchAdjustState adjustPatch = MS_10_SELECT;
    static uint8_t oscSelect = 0;
    static uint8_t testMidiNote = 0;
    // not implemented yet:
    // 1ms select = 'p'
    // 10ms select = '['
    // 100ms select = ']'
    // 10000ms select = '\'

    // implemented:
    // show FM channel parameters = 'j'
    // increase value = '+'
    // decrease value = '-'
    // select frequency = 'w'
    // select oscillator = 'q'
    // select algorithm = 'a'
    // select feedback = 'f'
    // select adsr attack rate (R0) = 'z'
    // select adsr decay rate (R1) = 'x'
    // select adsr release rate (R3) = 'c'
    // select adsr L3 = 'v'
    // select adsr L1 = 'b'
    // select adsr sustain = 'n'
    // select adsr L0 = 'm'
    // select oscillator ratio = 'l'
    // play note = ' ' (spacebar)
    //
    switch(selection) {
        case 'w':
          Serial1.printf("Change test MIDI note 440Hz or 880Hz.\n");
          if(!testMidiNote)
          {
            Serial1.printf("MIDI note 440Hz.\n");
            fmc[0].setFrequency(440.0);
            testMidiNote = 1;
          }
          else
          {
            Serial1.printf("MIDI note 880Hz.\n");
            fmc[0].setFrequency(880.0);
            testMidiNote = 0;
          }
        break;
        case 'h':
        case 'H':
          PatchMakerPrintHelp();
        break;
        case 'j':
        {
            fmc[0].printChannelDetails();
        }
        break;
        case '+':
        {
            float array[8];
            fmc[0].getOscDetails(oscSelect, array);
            if (selectPatch == ADSR_L0) {
                float L0 = array[ADSR_L0];
                L0 += 0.01;
                (L0 >= MAX_L0) ? L0 = MAX_L0 : L0;
                array[ADSR_L0] = L0;
                fmc[0].setOscDetails(oscSelect, array);
            }
            if (selectPatch == ADSR_L1) {
                float L1 = array[ADSR_L1];
                L1 += 0.01;
                (L1 >= MAX_L1) ? L1 = MAX_L1 : L1;
                array[ADSR_L1] = L1;
                fmc[0].setOscDetails(oscSelect, array);
            }
            if (selectPatch == ADSR_L3) {
                float L3 = array[ADSR_L3];
                L3 += 0.01;
                (L3 >= MAX_L3) ? L3 = MAX_L3 : L3;
                array[ADSR_L3] = L3;
                fmc[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == ADSR_R0) {
                float R0 = array[ADSR_R0];
                R0 += 0.01;
                (R0 >= MAX_R0) ? R0 = MAX_R0 : R0;
                array[ADSR_R0] = R0;
                fmc[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == ADSR_R1) {
                float R1 = array[ADSR_R1];
                R1 += 0.01;
                (R1 >= MAX_R1) ? R1 = MAX_R1 : R1;
                array[ADSR_R1] = R1;
                fmc[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == ADSR_R3) {
                float R3 = array[ADSR_R3];
                R3 += 0.01;
                (R3 >= MAX_R3) ? R3 = MAX_R3 : R3;
                array[ADSR_R3] = R3;
                fmc[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == ADSR_SUSTAIN) {
                float sustain = array[ADSR_SUSTAIN];
                sustain += 0.01;
                (sustain >= MAX_SUSTAIN) ? sustain = MAX_SUSTAIN : sustain;
                array[ADSR_SUSTAIN] = sustain;
                fmc[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == PATCH_RATIO) {
                float ratio = array[PATCH_RATIO];
                ratio += 0.01;
                (ratio >= MAX_RATIO) ? ratio = MAX_RATIO : ratio;
                array[PATCH_RATIO] = ratio;
                fmc[0].setOscDetails(oscSelect, array);
            }
            break;
        }
        case '-':
        {   
            float array[8];
            fmc[0].getOscDetails(oscSelect, array);
            if (selectPatch == ADSR_L0) {
                float L0 = array[ADSR_L0];
                L0 -= 0.01;
                (L0 <= 0.00) ? L0 = 0.00 : L0;
                array[ADSR_L0] = L0;
                fmc[0].setOscDetails(oscSelect, array);
            }
            if (selectPatch == ADSR_L1) {
                float L1 = array[ADSR_L1];
                L1 -= 0.01;
                (L1 <= 0.00) ? L1 = 0.00 : L1;
                array[ADSR_L1] = L1;
                fmc[0].setOscDetails(oscSelect, array);
            }
            if (selectPatch == ADSR_L3) {
                float L3 = array[ADSR_L3];
                L3 -= 0.01;
                (L3 <= 0.00) ? L3 = 0.00 : L3;
                array[ADSR_L3] = L3;
                fmc[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == ADSR_R0) {
                float R0 = array[ADSR_R0];
                R0 -= 0.01;
                (R0 <= 0.01) ? R0 = 0.01 : R0;
                array[ADSR_R0] = R0;
                fmc[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == ADSR_R1) {
                float R1 = array[ADSR_R1];
                R1 -= 0.01;
                (R1 <= 0.01) ? R1 = 0.01 : R1;
                array[ADSR_R1] = R1;
                fmc[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == ADSR_R3) {
                float R3 = array[ADSR_R3];
                R3 -= 0.01;
                (R3 <= 0.01) ? R3 = 0.01 : R3;
                array[ADSR_R3] = R3;
                fmc[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == ADSR_SUSTAIN) {
                float sustain = array[ADSR_SUSTAIN];
                sustain -= 0.01;
                (sustain <= 0.00) ? sustain = 0.00 : sustain;
                array[ADSR_SUSTAIN] = sustain;
                fmc[0].setOscDetails(oscSelect, array);
            }
            else if(selectPatch == PATCH_RATIO) {
                float ratio = array[PATCH_RATIO];
                ratio -= 0.01;
                (ratio <= 0.00) ? ratio = 0.00 : ratio;
                array[PATCH_RATIO] = ratio;
                fmc[0].setOscDetails(oscSelect, array);
            }
            break;
        }
        case '[':
            Serial1.printf("\n10ms selected!\n");
            adjustPatch = MS_10_SELECT;
            break;
        case 'v':
            Serial1.printf("\nADSR L3 selected!\n");
            selectPatch = ADSR_L3;
            break;
        case 'b':
            Serial1.printf("\nADSR L1 selected!\n");
            selectPatch = ADSR_L1;
            break;
        case 'm':
            Serial1.printf("\nADSR L0 selected!\n");
            selectPatch = ADSR_L0;
            break;
        case 'n':
            Serial1.printf("\nADSR sustain selected!\n");
            selectPatch = ADSR_SUSTAIN;
            break;
        case 'c':
            Serial1.printf("\nADSR release rate (R3) selected!\n");
            selectPatch = ADSR_R3;
            break;
        case 'x':
            Serial1.printf("\nADSR decay rate (R1) selected!\n");
            selectPatch = ADSR_R1;
            break;
        case 'z':
            Serial1.printf("\nADSR attack rate (R0) selected!\n");
            selectPatch = ADSR_R0;
            break;
        case 'f':
        {
            uint8_t fbShift = 0;
            Serial1.printf("\nselect feedback shift value (0-7): ");
            while(!Serial1.available());
            fbShift = Serial1.read() - 0x30;
            if (fbShift > 7) {
                fbShift = 7;
                Serial1.printf("\nMaximum feedback shift - setting to 7!\n");
            }
            else
            {
              Serial1.printf("\nFeedback %d selected!\n", fbShift);
              fmc[0].setFeedback(fbShift);
            }
            break;
        }
        case 'a':
        {
            uint8_t algorithm = 0;
            Serial1.printf("\nselect algorithm: ");
            while(!Serial1.available());
            algorithm = Serial1.read() - 0x30;
            if (algorithm > 8)
            {
                Serial1.printf("\ninvalid algorithm! 0 to 8 only!\n");
                break;
            }
            else
            {
                fmc[0].selectAlgorithm(algorithm);
                Serial1.printf("\nAlgorithm %d selected!\n", algorithm);
                break;
            }
        }
        case 'q':
        {
            uint8_t oscillator = 0;
            Serial1.printf("\nselect oscillator: ");
            while(!Serial1.available());
            oscillator = Serial1.read() - 0x30;
            if (oscillator > 3)
            {
                Serial1.printf("\noscillators 0-3 only!\n");
                break;
            } 
            else
            {
                Serial1.printf("\noscillator %d selected!\n", oscillator);
                oscSelect = oscillator;
                break;
            }
        }
        case 'l':
            Serial1.printf("\noscillator ratio selected!\n");
            selectPatch = PATCH_RATIO;
            break;
        case ' ':
            fmc[0].noteOn();
            break;
        default:
            Serial1.printf("Invalid selection!\n");
            break;
    }
}