#pragma once
#include "CAudioRecorder.h"
#include "TRingBuf.h"
#include "TWebRtcVad.h"

class CYyAudioRecorder : public CAudioRecorder
{
public:
	CYyAudioRecorder(unsigned int BuffSize = 16000*100);
	void process();
protected:
	TRingBuf m_ring;
	TWebRtcVad m_vad;
private:
	virtual int OnRecordCallback(const void* inputBuffer, void* outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags);
};

