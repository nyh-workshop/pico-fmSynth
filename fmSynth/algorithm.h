#ifndef _ALGORITHM_H
#define _ALGORITHM_H

#include "fmChannel.h"

// Using DX9 algorithms for 4-ops:
fp1516 algorithm2(fmChannel* fmc);
fp1516 algorithm3(fmChannel* fmc);
fp1516 algorithm5(fmChannel* fmc);
fp1516 algorithm7(fmChannel* fmc);

extern fp1516 (*algorithmList[])(fmChannel* fmc);

//typedef fp1516 (*fptrDict)(fmChannel*);
//extern std::map <int, fptrDict> algorithm_d;

#endif
