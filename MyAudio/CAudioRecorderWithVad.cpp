#include "CAudioRecorderWithVad.h"
#include <process.h>

CAudioRecorderWithVad::CAudioRecorderWithVad(unsigned int BuffSize):
	m_ring(BuffSize)
{

}

int CAudioRecorderWithVad::OnRecordCallback(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags) {
	
	m_ring.push((char*)inputBuffer, 
		framesPerBuffer * m_channelCount* m_SizePerFrame);
		
	return paContinue;
}
bool CAudioRecorderWithVad::startRecording(
	unsigned int sampleRate,
	PaSampleFormat sampleFormat,
	unsigned int channelCount) {
	::_beginthread(ThreadFun, 0, this);
	return CAudioRecorder::startRecording(sampleRate, sampleFormat, channelCount);
}
void __cdecl CAudioRecorderWithVad::ThreadFun(void* pThis) {
	((CAudioRecorderWithVad*)pThis)->process();
}
void  CAudioRecorderWithVad::process() {
	int  ms = 30;
	int  frameCount = m_sampleRate * ms / 1000;
	int BlockSize = frameCount * m_SizePerFrame;
	char* buf = (char*)malloc(BlockSize+100);
	
	int ms1 = 1000;
	int BlockSize1 = m_sampleRate * ms1 / 1000 * m_SizePerFrame;
	char* buf1 = (char*)malloc(BlockSize1+100);
	char  buf1Pos = 0;
	while (true) {
		m_ring.pop((char*)buf, BlockSize);
		int state = m_vad.check((int16_t*)buf, m_sampleRate, 480);
		if (1 == state) {
			memcpy(buf1+ buf1Pos, buf, BlockSize);
			buf1Pos += BlockSize;
			if (buf1Pos + BlockSize > BlockSize1) {
				OnData(buf1, buf1Pos);
				buf1Pos = 0;
			}
			printf(".");
		}
		else if (0 == state) {
			if (buf1Pos > 0) {
				OnData(buf1, buf1Pos);
				buf1Pos = 0;
			}
			printf(" ");
		}
		else {
			printf("x");
		}
	}
}

void CAudioRecorderWithVad::OnData(char* buf, int size) {

}