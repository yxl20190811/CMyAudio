#pragma once
#include "CAudioRecorder.h"
#include "TRingBuf.h"
#include "TWebRtcVad.h"

class CAudioRecorderWithVad : public CAudioRecorder
{
public:
	CAudioRecorderWithVad(unsigned int BuffSize = 16000*100);
	bool startRecording(
		unsigned int sampleRate,
		PaSampleFormat sampleFormat,
		unsigned int channelCount);
protected:
	TRingBuf m_ring;
	TWebRtcVad m_vad;
protected:
	static void __cdecl ThreadFun(void* pThis);
	void  process();
	virtual int OnRecordCallback(const void* inputBuffer, void* outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags);
	virtual void OnData(std::string& str);
};

