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
		//afPtr = algorithmList<int32_t>[patch.algorithm];
		afPtr = fnArray[patch.algorithm];
		
		if(!afPtr)
		{
			// If the algorithm is not available, assert.
			printf("algorithm not available!\n");
			assert(false);
		} 
		printf("Patch Name: %s\n", patch.name);
	}
}

// Algorithm0 is only for testing purposes only!
int32_t fmChannel::algorithm0() {
	int32_t Y0 = 0;

	Y0 = osc[0].opSineTest();

	return Y0;
}

int32_t fmChannel::algorithm1() {
	int32_t Y0 = 0;

	Y0 = osc[0].op(osc[1].op(osc[2].op(osc[3].opfb(getFeedback()))));

	return Y0;
}

int32_t fmChannel::algorithm2() {
	int32_t Y0 = 0;
	int32_t Y1 = 0;
	int32_t Y2 = 0;
	int32_t Y3 = 0;

	Y0 = osc[3].opfb(getFeedback());
	Y1 = osc[2].op((int32_t)0);
	Y2 = osc[1].op((Y0 + Y1) / (int32_t)2);
	Y3 = osc[0].op(Y2);

	return Y3;	
}

int32_t fmChannel::algorithm3() {
	int32_t Y0 = 0;
	int32_t Y1 = 0;
	int32_t Y2 = 0;
	int32_t Y3 = 0;

	Y0 = osc[2].op(osc[3].op((int32_t)0));
	Y1 = osc[1].opfb(getFeedback());
	Y2 = osc[0].op((Y0 + Y1) / (int32_t)2);

	return Y2;
}

int32_t fmChannel::algorithm4() {
	int32_t Y0 = 0;
	int32_t Y1 = 0;
	int32_t Y2 = 0;

	Y0 = osc[2].op(osc[3].opfb(getFeedback()));
	Y1 = osc[1].op(0);
	Y2 = osc[0].op(Y0 + Y1);

	return Y2;
}


int32_t fmChannel::algorithm5() {
	int32_t Y0 = 0;
	int32_t Y1 = 0;

	Y0 = osc[2].op(osc[3].opfb(getFeedback()));
	Y1 = osc[0].op(osc[1].op((int32_t)0));

	return (Y0 + Y1) / 2;
}

int32_t fmChannel::algorithm6() {
	int32_t Y0 = 0;
	int32_t Y1 = 0;

	Y0 = osc[3].opfb(getFeedback());
	Y1 = osc[2].op(Y0)+ osc[1].op(Y0) +	osc[0].op(Y0);

	return Y1 / 3;
}

int32_t fmChannel::algorithm7() {
	int32_t Y0 = 0;
	int32_t Y1 = 0;
	int32_t Y2 = 0;

	Y0 = osc[2].op(osc[3].opfb(getFeedback()));
	Y1 = osc[1].op((int32_t)0);
	Y2 = osc[0].op((int32_t)0);

	return (Y0 + Y1 + Y2) / 3;
}

int32_t fmChannel::algorithm8() {
	int32_t Y0 = 0;

	Y0 = osc[3].opfb(getFeedback()) + osc[2].op(0) + osc[1].op(0) + osc[0].op(0);

	return Y0 / 4;
}

int32_t fmChannel::generateSample() {
	int32_t output = 0;
	output = (this->*afPtr)();
	//output = (*afPtr)();
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
