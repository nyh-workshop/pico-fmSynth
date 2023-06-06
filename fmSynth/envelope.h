#ifndef _ENVELOPE_H
#define _ENVELOPE_H

#include "fmSynth.h"

const uint32_t tenMsTick = (uint32_t)(0.01f * (float)FMSYNTH_SAMPLE_RATE);

enum EnvelopeState { NONE = 0, ATTACK, DECAY, SUSTAIN, RELEASE };

template <typename T>
class Envelope {

public:
	Envelope();
	~Envelope();
	// Split into set levels (L) set rates (R) and set Sustain (sustain):
	// setRates and setLevels are in fixed point!
	// setRatesInSecs and setLevelsInFloat are in floating points!
	inline EnvelopeState getState() { return state; }
	void setRatesInSecs(float inputR0_s, float inputR1_s, float inputR3_s);
	void setRates(T inputR0, T inputR1, T inputR3);
	void setSustainInSecs(float inputSustain);
	void setLevelsInFloat(float inputL0_s, float inputL1_s, float inputL3_s);
	void setLevels(T inputL0, T inputL1, T inputL3);
	void setState(EnvelopeState inputState);
	void startADSR();
	void calculateEnvelope(T& _volume, uint32_t& _sustainCounter, EnvelopeState& _state);
	T envelopeStep();

private:
	T volume;
	T nextVolume;
	T lerpVol;
	// attack:
	T L0;
	T R0;
	// decay:
	T L1;
	T R1;
	// sustain (in number of samples):
	uint32_t sustain;
	uint32_t sustainCounter = 0;
	uint32_t nextSustainCounter = 0;
	// release:
	T L3;
	T R3;

	uint32_t envCount = 0;

	EnvelopeState state = NONE;
	EnvelopeState nextState = NONE;
};

template <typename T>
Envelope<T>::Envelope() {
	// all starts at zero at test!
	printf("Creating envelope!\n");

	// Compatible data types: float and fixed point!
	constexpr bool isFloat = std::is_same_v<T, float>;
	constexpr bool isFixedPoint = std::is_same_v<T, fixedPoint>;

	if constexpr (isFloat)
	{
		volume = 0.00f;
		nextVolume = 0.00f;
		L0 = 0.00f;
		R0 = 0.00f;
		L1 = 0.00f;
		R1 = 0.00f;
		L3 = 0.00f;
		R3 = 0.00f;
	}

	if constexpr (isFixedPoint)
	{
		volume = (fixedPoint)0.00f;
		nextVolume = (fixedPoint)0.00f;
		L0 = (fixedPoint)0.00f;
		R0 = (fixedPoint)0.00f;
		L1 = (fixedPoint)0.00f;
		R1 = (fixedPoint)0.00f;
		L3 = (fixedPoint)0.00f;
		R3 = (fixedPoint)0.00f;
	}

	sustain = 0;
}

template <typename T>
Envelope<T>::~Envelope() {
	printf("Deleting envelope!\n");
}

template <typename T>
void Envelope<T>::calculateEnvelope(T& _volume, uint32_t& _sustainCounter, EnvelopeState& _state) {

	// Compatible data types: float and fixed point!
	constexpr bool isFloat = std::is_same_v<T, float>;
	constexpr bool isFixedPoint = std::is_same_v<T, fixedPoint>;

    if (_state == ATTACK) {
			_volume += R0;
			//printf("a");
			//printf("%f ", (float)volume);
			if (_volume >= L0) {
				//printf(">");
				_volume = L0;
				_state = DECAY;
			}
		}
		else if (_state == DECAY) {
			//printf("d");
			//printf("%f ", (float)volume);
			_volume = _volume * R1;
			if (_volume <= L1) {
				_volume = L1;
				_state = SUSTAIN;
			}
		}
		else if (_state == SUSTAIN) {
			_sustainCounter = _sustainCounter + 1;
			if (_sustainCounter >= sustain) {
				_sustainCounter = 0;
				_state = RELEASE;
			}
		}
		else if (_state == RELEASE) {
			//printf("r");
			//printf("%f ", (float)volume);
			_volume = _volume * R3;
			if (_volume <= L3) {
				_state = NONE;
			}
		}
		else {
			// state is NONE here! no activity. :D
			//printf("n");
			if constexpr(isFloat)
				volume = 0.00f;
			if constexpr(isFixedPoint)
				volume = (int32_t)0;
		}
}

template <typename T>
T Envelope<T>::envelopeStep() {

	// Compatible data types: float and fixed point!
	constexpr bool isFloat = std::is_same_v<T, float>;
	constexpr bool isFixedPoint = std::is_same_v<T, fixedPoint>;
    
	// Calculating twice (current and next) because we need these for the linear interpolation! :D
	if (envCount >= tenMsTick) {
		envCount = 0;

        calculateEnvelope(volume, sustainCounter, state);
        
        nextVolume = volume;
        nextState = state;
        nextSustainCounter = sustainCounter;
        
        calculateEnvelope(nextVolume, nextSustainCounter, nextState);
        
        return volume;
	}
	else {
		// Linear interpolation here!
		if constexpr(isFloat)
		    lerpVol = ( float((nextVolume - volume)/tenMsTick) * envCount ) + volume;
		if constexpr(isFixedPoint) {
			fixedPoint tenMsTickFP = (fixedPoint)(float)(1/tenMsTick);
			lerpVol = ((nextVolume - volume)/tenMsTickFP) + volume;
		}
		
		envCount++;
		return lerpVol;
		//return volume;
	}
}

template <typename T>
void Envelope<T>::setLevels(T inputL0, T inputL1, T inputL3) {
	L0 = inputL0;
	L1 = inputL1;
	L3 = inputL3;
}

template <typename T>
void Envelope<T>::setLevelsInFloat(float inputL0_s, float inputL1_s, float inputL3_s) {

	// Compatible data types: float and fixed point!
	constexpr bool isFloat = std::is_same_v<T, float>;
	constexpr bool isFixedPoint = std::is_same_v<T, fixedPoint>;

	if constexpr (isFloat)
	{
		L0 = inputL0_s;
		L1 = inputL1_s;
		L3 = inputL3_s;
	}

	if constexpr (isFixedPoint)
	{
		L0 = (fixedPoint)inputL0_s;
		L1 = (fixedPoint)inputL1_s;
		L3 = (fixedPoint)inputL3_s;
	}
}

template <typename T>
void Envelope<T>::setRates(T inputR0, T inputR1, T inputR3) {
	R0 = inputR0;
	R1 = inputR1;
	R3 = inputR3;
}

template <typename T>
void Envelope<T>::setRatesInSecs(float inputR0_s, float inputR1_s, float inputR3_s) {

	// Compatible data types: float and fixed point!
	constexpr bool isFloat = std::is_same_v<T, float>;
	constexpr bool isFixedPoint = std::is_same_v<T, fixedPoint>;

	// ... the divide by 0.01 because we only do the envelope every 10ms!
	// set for attack:
	float numOfSamplesForAttack = inputR0_s / (float)(0.01f);
	float temp0 = ((float)L0 / numOfSamplesForAttack);
	//printf("temp0: %f\n", temp0);
	//R0 = (T)temp0;

	// set for decay:
	float numOfSamplesForDecay = inputR1_s / (float)(0.01f);
	float temp1 = (-log((float)L1) / numOfSamplesForDecay);
	printf("numOfSamplesForDecay: %f\n", numOfSamplesForDecay);
	//printf("temp1: %f\n", temp1);
	//R1 = (T)exp(-temp1);

	// set for release:
	float numOfSamplesForRelease = inputR3_s / (float)(0.01f);
	float temp2 = (-log((float)L3) / numOfSamplesForRelease);
	//printf("temp2: %f\n", temp2);
	//R3 = (T)exp(-temp2);

	if constexpr (isFloat)
	{
		R0 = temp0;
		R1 = exp(-temp1);
		R3 = exp(-temp2);
	}

	if constexpr (isFixedPoint)
	{
		R0 = (fixedPoint)temp0;
		R1 = (fixedPoint)exp(-temp1);
		R3 = (fixedPoint)exp(-temp2);
	}

	printf("R0: %f\n", (float)R0);
	printf("R1: %f\n", (float)R1);
	printf("R3: %f\n", (float)R3);
}

template <typename T>
void Envelope<T>::setState(EnvelopeState inputState) {
	
	// Compatible data types: float and fixed point!
	constexpr bool isFloat = std::is_same_v<T, float>;
	constexpr bool isFixedPoint = std::is_same_v<T, fixedPoint>;

	state = inputState;
	envCount = 0;
}

template <typename T>
void Envelope<T>::setSustainInSecs(float inputSustain) {
	sustain = (uint32_t)(inputSustain / 0.01f);
}

template <typename T>
void Envelope<T>::startADSR() {
    setState(ATTACK);
    envCount = 0;

    nextVolume = volume;
    nextState = state;
    nextSustainCounter = sustainCounter;
        
    calculateEnvelope(nextVolume, nextSustainCounter, nextState);
}

#endif