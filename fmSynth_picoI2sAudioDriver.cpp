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

fmSynthPicoI2s::fmSynthPicoI2s(bool testMode) {
  Serial1.println("Configure interpolator lanes...");
  configureInterpLanes();
  Serial1.println("Start up fmSynth I2S...");
  if (testMode == true) {
    Serial1.println("Start up fmSynth I2S Test mode...");
    for (uint8_t i = 0; i < MAX_FM_CHANNELS; i++) {
      fmc[i].setChannelInstrument("TEST00");
    }
  } else {
    for (uint8_t i = 0; i < MAX_FM_CHANNELS; i++) {
      fmc[i].setChannelInstrument("GUITAR");
    }
  }
}

fmSynthPicoI2s::~fmSynthPicoI2s() {
  Serial1.println("closing fmSynthPicoI2s...");
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
  return tempSample;
}