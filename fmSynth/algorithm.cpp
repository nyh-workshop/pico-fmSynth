#include "algorithm.h"

fp1516 algorithm3(fmChannel* fmc) {
	fp1516 Y0 = (int32_t)0;
	fp1516 Y1 = (int32_t)0;
	fp1516 Y2 = (int32_t)0;
	fp1516 Y3 = (int32_t)0;

	Y0 = fmc->osc[2].op(fmc->osc[3].op((int32_t)0));
	Y1 = fmc->osc[1].opfb(fmc->getFeedback());
	Y2 = fmc->osc[0].op((Y0 + Y1) / (fp1516)((int32_t)2));

	return Y2;
}

fp1516 algorithm2(fmChannel* fmc) {
	fp1516 Y0 = (int32_t)0;
	fp1516 Y1 = (int32_t)0;
	fp1516 Y2 = (int32_t)0;
	fp1516 Y3 = (int32_t)0;

	Y0 = fmc->osc[3].opfb(fmc->getFeedback());
	Y1 = fmc->osc[2].op((int32_t)0);
	Y2 = fmc->osc[1].op((Y0 + Y1) / (fp1516)((int32_t)2));
	Y3 = fmc->osc[0].op(Y2);

	return Y3;
}

fp1516 algorithm5(fmChannel* fmc) {
	fp1516 Y0 = (int32_t)0;
	fp1516 Y1 = (int32_t)0;

	Y0 = fmc->osc[2].op(fmc->osc[3].opfb(fmc->getFeedback()));
	Y1 = fmc->osc[0].op(fmc->osc[1].op((int32_t)0));

	return (Y0 + Y1) / 2;
}

fp1516 algorithm7(fmChannel* fmc) {
	fp1516 Y0 = (int32_t)0;
	fp1516 Y1 = (int32_t)0;
	fp1516 Y2 = (int32_t)0;

	Y0 = fmc->osc[2].op(fmc->osc[3].opfb(fmc->getFeedback()));
	Y1 = fmc->osc[1].op((int32_t)0);
	Y2 = fmc->osc[0].op((int32_t)0);

	return (Y0 + Y1 + Y2) / 3;
}

fp1516 (*algorithmList[])(fmChannel *fmc) = {nullptr,
											 nullptr,
											 algorithm2,
											 algorithm3,
											 nullptr,
											 algorithm5,
											 nullptr,
											 algorithm7,
											 nullptr};

// std::map <int, fptrDict> algorithm_d = { {2, &algorithm2},
// 										 {3, &algorithm3},
// 										 {5, &algorithm5},
// 										 {7, &algorithm7} };
