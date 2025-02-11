#include "oscillator.h"

int16_t wavetable1024[1024];

bool Oscillator::wavetableFilled = false;

constexpr uint32_t SINE_440_TUNING_WORD = ((uint64_t)UINT32_MAX * 440ul) / FMSYNTH_SAMPLE_RATE; 

Oscillator::Oscillator() {
	Serial1.println("osc create!");

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
	Serial1.println("osc delete!");
}

void Oscillator::setFrequency(float inputFreq) {
	tuningWord = ceil(inputFreq * pow(2, 32) / (float)FMSYNTH_SAMPLE_RATE);
	//printf("tuningWord: %f\n", (float)tuningWord);
}

int32_t Oscillator::op(int32_t inputFeedback)
{

	if (adsr.getState() != NONE)
	{
		interp1->base[0] = tuningWord;
		interp1->accum[0] = accumulator;

		uint32_t _result2 = interp_pop_full_result(interp1);
		// int32_t _result0 = wavetable1024[(_result2 >> 22)];
		int32_t _result0 = wavetable1024[((_result2 >> 22) + inputFeedback) & 0x3ff];

		accumulator = interp1->accum[0];

		// I don't even know what's making that screeching noise when Fixed Point is enabled for RP2350!
    #if defined(RP2040_SDK)
		fixedPoint outputFP(_result0);
    int32_t output = (int32_t)(outputFP * adsr.envelopeStep());    
    #elif defined(RP2350_ARDUINO)
    float outputFloat = float(_result0);
    int32_t output = (int32_t)(outputFloat * adsr.envelopeStep());
    #endif

		return output;
	}
	else
		return 0;
}

int32_t Oscillator::opfb(uint8_t fbShift) {

	if (adsr.getState() != NONE)
	{
		int32_t scaled_fb = (feedback[0] + feedback[1]) >> (fbShift + 1);
		feedback[1] = feedback[0];

		interp1->base[0] = tuningWord;
		interp1->accum[0] = accumulator;

		uint32_t _result2 = interp_pop_full_result(interp1);
		int32_t _result0 = wavetable1024[((_result2 >> 22) + scaled_fb) & 0x3ff];

		feedback[0] = _result0;

		accumulator = interp1->accum[0];
		
    // I don't even know what's making that screeching noise when Fixed Point is enabled for RP2350!
    #if defined(RP2040_SDK)
		fixedPoint outputFP(_result0);
    int32_t output = (int32_t)(outputFP * adsr.envelopeStep());    
    #elif defined(RP2350_ARDUINO)
    float outputFloat = float(_result0);
    int32_t output = (int32_t)(outputFloat * adsr.envelopeStep());
    #endif

		return output;
	}
	else
		return 0;
}

void Oscillator::clearFeedbackArray() {
	// Clears all the feedback array. Useful for patch debug purpose where the feedbacks have to be cleared after switching algorithms.
	feedback[0] = 0;
	feedback[1] = 0;
}

int32_t Oscillator::opSineTest()
{
	// This test is outputting samples of pure sine wave, with the tuningWord.
	// Note: If there aren't any outputs or wrong outputs, check whether the interpolators are properly initialized or not!

	absolute_time_t before = get_absolute_time();

	interp1->base[0] = SINE_440_TUNING_WORD;
	interp1->accum[0] = accumulator;

	// printf("osc address: %08x\n", this);
	// printf("before accumulator: %d\n", accumulator);
	uint32_t _result2 = interp_pop_full_result(interp1);
	int32_t _result0 = wavetable1024[(_result2 >> 22) & 0x3ff];
	accumulator = interp1->accum[0];
	// printf("after accumulator: %d\n", accumulator);
	// printf("%d ,", _result0);

	absolute_time_t after = get_absolute_time();

	// printf("op: time taken -> %d\n", absolute_time_diff_us(before, after));

	return _result0;
}

int32_t Oscillator::opSineFbTest(uint8_t fbShift)
{
	// This test is outputting samples of sine wave with feedback, with the tuningWord.
	// Set the tuningWord before calling this.
	// Note: If there aren't any outputs or wrong outputs, check whether the interpolators are properly initialized or not!

	absolute_time_t before = get_absolute_time();

	int32_t scaled_fb = (feedback[0] + feedback[1]) >> (fbShift + 1);
	feedback[1] = feedback[0];

	interp1->base[0] = SINE_440_TUNING_WORD;
	interp1->accum[0] = accumulator;

	uint32_t _result2 = interp_pop_full_result(interp1);
	int32_t _result0 = wavetable1024[((_result2 >> 22) + scaled_fb) & 0x3ff];

	printf("%d ,", _result0);

	feedback[0] = _result0;

	accumulator = interp1->accum[0];

	absolute_time_t after = get_absolute_time();

	return _result0;
}