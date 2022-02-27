#include "oscillator.h"

int16_t wavetable1024[1024];

bool Oscillator::wavetableFilled = false;

Oscillator::Oscillator() {
	printf("osc create!\n");

	// Fill this wavetable once! :D
	if (!wavetableFilled)
	{
		// Fill up this n-point wavetable!
		for (int i = 0; i < 1024; i++)
		{
			wavetable1024[i] = (int16_t)(162.97 * sin(((float)i / (float)(1024.0) * 2 * M_PI)));
			// printf("%d, ", wavetable1024[i]);
		}
		wavetableFilled = true;
	}
	adsr.setRates((int32_t)0, (int32_t)0, (int32_t)0);
	adsr.setLevels((int32_t)0, (int32_t)0, (int32_t)0);
	accumulator = 0;
	tuningWord = 0;
}

Oscillator::~Oscillator() {
	printf("osc delete!\n");
}

void Oscillator::configureInterpLanes() {
	interp_config interpCfg = interp_default_config();

	// Lane 0 settings:
    interp_config_set_add_raw(&interpCfg, true);
    interp_config_set_cross_input(&interpCfg, 0);
    interp_config_set_mask(&interpCfg, 22, 31);
    interp_config_set_shift(&interpCfg, 0);
    interp_config_set_signed(&interpCfg, false);
    interp_set_config(interp0, 0, &interpCfg);
    // Lane 1 settings:
    interp_config_set_add_raw(&interpCfg, true);
    interp_config_set_cross_input(&interpCfg, 0);
    interp_config_set_mask(&interpCfg, 0, 9);
    interp_config_set_shift(&interpCfg, 22);
    interp_config_set_signed(&interpCfg, false);
    interp_set_config(interp0, 1, &interpCfg);
}

void Oscillator::setFrequency(float inputFreq) {
	tuningWord = ceil(inputFreq * pow(2, 32) / (float)SAMPLE_RATE);
	//printf("tuningWord: %f\n", (float)tuningWord);
}

int32_t Oscillator::op(int32_t inputFeedback) {

	//configureInterpLanes();

	interp0->base[0] = tuningWord;
	interp0->accum[0] =  accumulator;	

	uint32_t _result2 = interp_pop_full_result(interp0);	
	//int32_t _result0 = wavetable1024[(_result2 >> 22)];
	int32_t _result0 = wavetable1024[((_result2 >> 22) + inputFeedback) & 0x3ff];

	accumulator = interp0->accum[0];

	fixedPoint outputFP(_result0);

	int32_t output = (int32_t)(outputFP * adsr.envelopeStep());

	return output;
}

int32_t Oscillator::opfb(uint8_t fbShift) {

	int32_t scaled_fb = (feedback[0] + feedback[1]) >> (fbShift + 1);
    feedback[1] = feedback[0];

	interp0->base[0] = tuningWord;
	interp0->accum[0] =  accumulator;

	uint32_t _result2 = interp_pop_full_result(interp0);
	int32_t _result0 = wavetable1024[( (_result2 >> 22) + scaled_fb ) & 0x3ff];

	feedback[0] = _result0;

	accumulator = interp0->accum[0];

	fixedPoint outputFP(_result0);

	int32_t output = (int32_t)(outputFP * adsr.envelopeStep());

	return output;

	// debugging values here!
	//printf("acc: %f, output: %f\n", (float)accumulator, (float)outputFP);
	//printf("%f ", (float)time);
	//printf("%d ", sampleTime);
	//printf("%f ", (float)accumulator);
	//printf("%f ", (float)outputFP);
}

int32_t Oscillator::opTest() {
	// This test is outputting samples of pure sine wave, with the tuningWord.
	// Set the tuningWord before calling this.
	// Note: If there aren't any outputs or wrong outputs, check whether the interpolators are properly initialized or not!

	absolute_time_t before = get_absolute_time();

	interp0->base[0] = tuningWord;
	interp0->accum[0] = accumulator;
	//printf("osc address: %08x\n", this);
	//printf("before accumulator: %d\n", accumulator);
	uint32_t _result2 = interp_pop_full_result(interp0);
	int32_t _result0 = wavetable1024[(_result2 >> 22)];
	accumulator = interp0->accum[0];
	//printf("after accumulator: %d\n", accumulator);
	//printf("%d ,", _result0);

	absolute_time_t after = get_absolute_time();

	printf("op: time taken -> %d\n", absolute_time_diff_us(before, after));

	return _result0;
}