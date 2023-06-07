#include "fmSynth_picoI2sAudioDriver.h"

fmSynthPicoI2s::fmSynthPicoI2s()
{
    for(uint8_t i = 0; i < MAX_FM_CHANNELS; i++)
    {
        fmc[i].setChannelInstrument("E.PIANO");
    }    
}

int16_t fmSynthPicoI2s::getSample()
{
    int16_t tempSample = (int16_t)0;
    for (uint8_t chnNum = 0; chnNum < MAX_FM_CHANNELS; chnNum++)
    {
        //absolute_time_t before = get_absolute_time();
        tempSample += 128 * fmc[chnNum].generateSample() / MAX_FM_CHANNELS;
        // absolute_time_t after = get_absolute_time();
        // printf("generate fm sample time: %d\n", (uint32_t)absolute_time_diff_us(before, after))
    }
    return tempSample;
}