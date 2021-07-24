#include "envelope.h"

Envelope::Envelope() {
	// all starts at zero at test!
	printf("Creating envelope!\n");
	volume = (int32_t)0;
	L0 = (int32_t)0;
	R0 = (int32_t)0;
	L1 = (int32_t)0;
	R1 = (int32_t)0;
	sustain = (int32_t)0;
	L3 = (int32_t)0;
	R3 = (int32_t)0;
}

Envelope::~Envelope() {
	printf("Deleting envelope!\n");
}

void Envelope::setRatesInSecs(float inputR0_s, float inputR1_s, float inputR3_s) {
	// ... the divide by 0.01 because we only do the envelope every 10ms!
	// set for attack:
	float numOfSamplesForAttack = inputR0_s / (float)(0.01f);
	float temp0 = ((float)L0 / numOfSamplesForAttack);
	printf("temp0: %f\n", temp0);
	R0 = (fp1516)temp0;

	// set for decay:
	float numOfSamplesForDecay = inputR1_s / (float)(0.01f);
	float temp1 = (-log((float)L1) / numOfSamplesForDecay);
	printf("numOfSamplesForDecay: %f\n", numOfSamplesForDecay);
	printf("temp1: %f\n", temp1);
	R1 = (fp1516)exp(-temp1);

	// set for release:
	float numOfSamplesForRelease = inputR3_s / (float)(0.01f);
	float temp2 = (-log((float)L3) / numOfSamplesForRelease);
	printf("temp2: %f\n", temp2);
	R3 = (fp1516)exp(-temp2);

	printf("R0: %f\n", (float)R0);
	printf("R1: %f\n", (float)R1);
	printf("R3: %f\n", (float)R3);
}

void Envelope::setLevelsInFloat(float inputL0_s, float inputL1_s, float inputL3_s) {
	L0 = (fp1516)inputL0_s;
	L1 = (fp1516)inputL1_s;
	L3 = (fp1516)inputL3_s;	
}

void Envelope::setRates(fp1516 inputR0, fp1516 inputR1, fp1516 inputR3) {
	R0 = inputR0;
	R1 = inputR1;
	R3 = inputR3;
}

void Envelope::setLevels(fp1516 inputL0, fp1516 inputL1, fp1516 inputL3) {
	L0 = inputL0;
	L1 = inputL1;
	L3 = inputL3;
}

void Envelope::setState(EnvelopeState inputState) {
	// todo: change this to restart envelope!
	//volume = (fp1516)0.000f;
	state = inputState;
	sustainCounter = 0;
	envCount = 0;
}

void Envelope::setSustainInSecs(float inputSustain) {
	sustain = (uint32_t)(inputSustain / 0.01f);
}

fp1516 Envelope::envelopeStep() {

	if (envCount >= tenMsTick) {
		envCount = 0;

		if (state == ATTACK) {
			volume += R0;
			//printf("a");
			//printf("%f ", (float)volume);
			if (volume >= L0) {
				//printf(">");
				volume = L0;
				state = DECAY;
			}
		}
		else if (state == DECAY) {
			//printf("d");
			//printf("%f ", (float)volume);
			volume = volume * R1;
			if (volume <= L1) {
				volume = L1;
				state = SUSTAIN;
			}
		}
		else if (state == SUSTAIN) {
			sustainCounter = sustainCounter + 1;
			if (sustainCounter >= sustain) {
				sustainCounter = 0;
				state = RELEASE;
			}
		}
		else if (state == RELEASE) {
			//printf("r");
			//printf("%f ", (float)volume);
			volume = volume * R3;
			if (volume <= L3) {
				state = NONE;
			}
		}
		else {
			// state is NONE here! no activity. :D
			//printf("n");
			volume = (fp1516)0.00f;
		}
		return volume;
	}
	else {
		envCount++;
		return volume;
	}
}
