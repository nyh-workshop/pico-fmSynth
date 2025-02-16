#ifndef _FMCHANNEL_H
#define _FMCHANNEL_H

#include "oscillator.h"
#include "patch.h"

enum sineTestType { SINE_ONLY = 0,
                    SINE_WITH_FEEDBACK };

class fmChannel {
public:
  using algorithmPtr = int32_t (fmChannel::*)();

  // Lumping the algorithms into the FM channel:
  int32_t algorithm0();
  int32_t algorithm1();
  int32_t algorithm2();
  int32_t algorithm3();
  int32_t algorithm4();
  int32_t algorithm5();
  int32_t algorithm6();
  int32_t algorithm7();
  int32_t algorithm8();

  // Default fm channel settings: all zeros!
  fmChannel();
  void setChannelInstrument(std::string patchName);
  void noteOn();
  void noteOff();
  void setFrequency(float inputFreq);
  uint8_t getFeedback();
  void setFeedback(uint8_t fbShift);
  int32_t generateSample();
  int32_t generateTestSineSample();
  void selectAlgorithm(uint8_t algorithmNum);

  // For Midi Player only:
  inline void setIsPlaying() { isPlaying = true; }
  inline void clearIsPlaying() { isPlaying = false; }
  inline bool getIsPlaying() { return isPlaying; }
  inline void setMidiNoteNum(uint8_t num) { midiNote = num; }
  inline uint8_t getMidiNoteNum() { return midiNote; }

  // For patch diagnostics only!
  void printChannelDetails();
  void getOscDetails(uint8_t osc, float* array);
  void setOscDetails(uint8_t osc, float* array);

  // Temporary fix for this one - 4 oscillators maximum for each FM channel!
  static const uint8_t MAX_OSC = 4;
  Oscillator osc[MAX_OSC];
private:
  float fmChannelFreqInHz;
  int32_t fmChannelOutput;
  
  bool isPlaying = false;
  uint8_t midiNote = 0;
  
  fmPatch patch;
  sineTestType sineTest = SINE_ONLY;

  //https://stackoverflow.com/questions/57926015/how-to-declare-an-array-of-pointers-to-member-functions-in-c
 static constexpr algorithmPtr fnArray[16] = { &fmChannel::algorithm0, &fmChannel::algorithm1, &fmChannel::algorithm2, &fmChannel::algorithm3, &fmChannel::algorithm4, &fmChannel::algorithm5, &fmChannel::algorithm6, &fmChannel::algorithm7, &fmChannel::algorithm8 };

  algorithmPtr afPtr;
};

#endif
