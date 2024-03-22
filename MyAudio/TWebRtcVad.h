#pragma once
#include "vad.h"
class TWebRtcVad
{
public:
	TWebRtcVad(int WebRtcVadMode=3);
	~TWebRtcVad();
	int check(const int16_t* audio_frame,
		size_t sampleRate,
		size_t frame_length);
private:
	VadInst* m_vadInst;
};

