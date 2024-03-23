#pragma once
#include "CAudioRecorderWithVad.h"
#include "TBaiduAudio.h"
class TAudioRec2Baidu: 
	protected CAudioRecorderWithVad, 
	protected TBaiduAudio
{
public:
	virtual void OnData(std::vector<uint8_t>& str);
	void start(unsigned int sampleRate,
		PaSampleFormat sampleFormat,
		unsigned int channelCount);
};

