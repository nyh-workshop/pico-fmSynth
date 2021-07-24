#include "oscillator.h"

Oscillator::Oscillator() {
	printf("osc create!\n");
	printf("oscPiFP: %x\n", oscPiFP.GetRawVal());
	adsr.setRates((int32_t)0, (int32_t)0, (int32_t)0);
	adsr.setLevels((int32_t)0, (int32_t)0, (int32_t)0);
	accumulator = (int32_t)0;
	accumulatorFP = (int32_t)0;
	freqFP = (int32_t)0;
	tuningWord = (int32_t)0;
	tempFP = (int32_t)0;
	sampleTime = (int32_t)0;
	//oscPiFP = M_PI;
}

Oscillator::~Oscillator() {
	printf("osc delete!\n");
}

fp1516& Oscillator::setTempFP() {
	return this->tempFP;
}

void Oscillator::setFrequency(float inputFreq) {
	//freqFP = inputFreq;
	tuningWord = ceil(inputFreq * pow(2, 32) / (float)SAMPLE_RATE);
	//tuningWord = freqFP / sampleRateFP;
	//printf("tuningWord: %f\n", (float)tuningWord);
}

inline fp1516 Oscillator::bhaskaraOneSine(fp1516 inputFP) {
	// https://datagenetics.com/blog/july12019/index.html
	// using Bhaskara I algorithm:
	// sin(x) = 16x(pi-x)/(5pi^2 - 4x(pi-x))
	// range: [0, pi)

	fp1516 piMinusX(0.00);
	const fp1516 fivePiSquared(5 * M_PI * M_PI);
	const fp1516 sixteen(16.0);
	const fp1516 four(4.0);
	fp1516 outputFP(0.00);

	piMinusX = oscPiFP - inputFP;

	outputFP = (sixteen * inputFP * piMinusX) / (fivePiSquared - four * inputFP * piMinusX);
	//printf("outputFP: %f\n", (float)outputFP);
	//printf("acc: %f tuningWord: %f inputFP: %f, outputFP: %f, piMinusX: %f oscPiFP: %f\n", (float)accumulator, (float)tuningWord, (float)inputFP, (float)outputFP, (float)piMinusX, (float)oscPiFP);

	//if ((float)outputFP > 1.0) {
	//	// test it in python: (16*x*(math.pi-x)/(5*math.pi*math.pi - 4*x*(math.pi-x)))
	//	printf("invalid sine output!\n");
	//	printf("piMinusX = %f\n", (float)piMinusX);
	//	printf("fivePiSquared = %f\n", (float)fivePiSquared);
	//	printf("sixteen = %f\n", (float)sixteen);
	//	printf("four = %f\n", (float)four);
	//	assert(0);
	//}
	//printf("%f\n", (float)outputFP);
	return outputFP;
}

inline fp1516 Oscillator::calculateSine(fp1516 inputFP) {

	const fp1516 zeroPointFive(0.5);
	const fp1516 PiFP(M_PI);
	const fp1516 twoPiFP(2 * M_PI);
	fp1516 outputFP = (int32_t)0;

	if (inputFP > PiFP) {
		//inputFP = (twoPiFP * inputFP) - PiFP;
		inputFP = inputFP - PiFP;
		outputFP = -bhaskaraOneSine(inputFP);
	}
	else
	{
		outputFP = bhaskaraOneSine(inputFP);
	}

	return outputFP;
}

fp1516 Oscillator::op(fp1516 inputFeedback) {
	fp1516 tempFP(0.0);
	fp1516 outputFP((int32_t)0);
	const fp1516 twoPiFP(2 * M_PI);
	fp1516 remainder((int32_t)0);
	int32_t temp;

	accumulator += tuningWord;
	temp = accumulator >> 20;
	tempFP = (fp1516)temp * twoPiFP / (fp1516)((int32_t)4096);
	tempFP += inputFeedback;
	accumulatorFP = tempFP;

	outputFP = calculateSine(tempFP);
	outputFP = outputFP * adsr.envelopeStep();
	//printf("%f ", (float)adsr.envelopeStep());

	return outputFP;
}

fp1516 Oscillator::opfb(uint8_t fbShift) {

	fp1516 tempFP(0.0);
	fp1516 outputFP((int32_t)0);
	const fp1516 twoPiFP(2 * M_PI);
	fp1516 remainder((int32_t)0);
	int32_t temp;
	fp1516 inputFeedback;

	inputFeedback = (feedbackFP[0] + feedbackFP[1]) / (fp1516)((int32_t)1 << ((int32_t)fbShift + (int32_t)1));
	feedbackFP[1] = feedbackFP[0];

	accumulator += tuningWord;
	temp = accumulator >> 20;
	tempFP = (fp1516)temp * twoPiFP / (fp1516)((int32_t)4096);
	tempFP += inputFeedback;
	accumulatorFP = tempFP;

	feedbackFP[0] = accumulatorFP;

	//if (accumulator < (fp1516)0.00f) {
	//	printf("accumulator cannot be less than 0!!\n");
	//	printf("2pi: %f, freqFP: %f\n", (float)twoPiFP, (float)freqFP);
	//	printf("acc (fixed): %x\n", (int32_t)accumulator.GetRawVal());
	//	printf("acc: %f, output: %f\n", (float)accumulator, (float)outputFP);
	//	printf("time: %f, sampleTime: %d\n", (float)time, sampleTime);
	//	printf("inputfeedback: %f\n", (float)inputFeedback);
	//	assert(0);
	//}

	outputFP = calculateSine(tempFP);
	outputFP = outputFP * adsr.envelopeStep();

	// debugging values here!
	//printf("acc: %f, output: %f\n", (float)accumulator, (float)outputFP);
	//printf("%f ", (float)time);
	//printf("%d ", sampleTime);
	//printf("%f ", (float)accumulator);
	//printf("%f ", (float)outputFP);

	return outputFP;
}