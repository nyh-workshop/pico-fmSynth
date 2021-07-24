#include "algorithm.h"
#include "fmChannel.h"

fmChannel::fmChannel() {
	// all the items in patch are zero!
	patch.name = (char*)"Default";
	//patch.name = "Default";
	patch.algorithm = 0;
	for (uint8_t i = 0; i < MAX_OSC; i++) {
		patch.ops[i].L0 = 0;
		patch.ops[i].L1 = 0;
		patch.ops[i].L3 = 0;
		patch.ops[i].R0 = 0;
		patch.ops[i].R1 = 0;
		patch.ops[i].R3 = 0;
		patch.ops[i].sustainInSecs = 0.0f;
		patch.ops[i].ratio = 0.0f;
	}
}

fmChannel::fmChannel(std::string patchName) {

	struct fmPatch inputPatch;

	//stdio_init_all();

	patch.name = (char*)"Default";
	patch.algorithm = 0;

	// Search for the patch name in the patch list:
	for(auto i : fmPatchList) {
		//if(!strcmp(i.name,patchName.c_str())) {
		if(patchName == i.name)	{
			patch.name = (char*)patchName.c_str();
			inputPatch = (fmPatch)i;
			break;
		}
	}

	if (patch.name == "Default")
	{
		// No such patch found! Assert here!
		assert(false);
	}
	else
	{	
		patch.algorithm = inputPatch.algorithm;
		patch.feedback = inputPatch.feedback;
		for (uint8_t i = 0; i < MAX_OSC; i++) {
			patch.ops[i] = inputPatch.ops[i];
		}
		for (uint8_t i = 0; i < MAX_OSC; i++) {
			printf("%f %f %f \n", patch.ops[i].R0, patch.ops[i].R1, patch.ops[i].R3);
			// Load the levels first!
			osc[i].adsr.setLevelsInFloat(patch.ops[i].L0, patch.ops[i].L1, patch.ops[i].L3);
			osc[i].adsr.setRatesInSecs(patch.ops[i].R0, patch.ops[i].R1, patch.ops[i].R3);
			osc[i].adsr.setSustainInSecs(patch.ops[i].sustainInSecs);
		}
		//afPtr = inputAfPtr;
		afPtr = algorithmList[patch.algorithm];
		if(afPtr == nullptr)
		{
			// If the algorithm is not available, assert.
			assert(false);
		} 
		printf("Patch Name: %s\n", patch.name);
	}
}

// fmChannel::fmChannel(fmPatch inputPatch, fp1516 (*inputAfPtr)(fmChannel*)) {
// 	patch.name = inputPatch.name;
// 	patch.algorithm = inputPatch.algorithm;
// 	patch.feedback = inputPatch.feedback;
// 	for (uint8_t i = 0; i < MAX_OSC; i++) {
// 		patch.ops[i] = inputPatch.ops[i];
// 	}
// 	for (uint8_t i = 0; i < MAX_OSC; i++) {
// 		printf("%f %f %f \n", patch.ops[i].R0, patch.ops[i].R1, patch.ops[i].R3);
// 		// Load the levels first!
// 		osc[i].adsr.setLevelsInFloat(patch.ops[i].L0, patch.ops[i].L1, patch.ops[i].L3);
// 		osc[i].adsr.setRatesInSecs(patch.ops[i].R0, patch.ops[i].R1, patch.ops[i].R3);		
// 		osc[i].adsr.setSustainInSecs(patch.ops[i].sustainInSecs);
// 	}
// 	afPtr = inputAfPtr;
// 	printf("Patch Name: %s\n", patch.name.c_str());
// }

fp1516 fmChannel::generateSample() {
	fp1516 output = (int32_t)0;
	output = (*afPtr)(this);
	return output;
}

uint8_t fmChannel::getFeedback() {
	return patch.feedback;
}

void fmChannel::noteOn() {
	for (uint8_t i = 0; i < MAX_OSC; i++) {
		osc[i].adsr.setState(ATTACK);
	}
}

void fmChannel::noteOff() {
	for (uint8_t i = 0; i < MAX_OSC; i++) {
		osc[i].adsr.setState(RELEASE);
	}
}

void fmChannel::setFrequency(float inputFreq) {
	for (uint8_t i = 0; i < MAX_OSC; i++) {
		osc[i].setFrequency(inputFreq * patch.ops[i].ratio);
	}
}
