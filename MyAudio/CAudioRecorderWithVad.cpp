#include "CAudioRecorderWithVad.h"
#include <process.h>
#include <vector>
CAudioRecorderWithVad::CAudioRecorderWithVad(unsigned int BuffSize):
	m_ring(BuffSize)
{

}

int CAudioRecorderWithVad::OnRecordCallback(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags) {
	/*
	std::string str1;
	str1.resize(framesPerBuffer * m_channelCount * m_SizePerFrame);
	memcpy((char*)str1.c_str(), inputBuffer, framesPerBuffer * m_channelCount * m_SizePerFrame);
	OnData(str1);
	*/
	m_ring.push((char*)inputBuffer, framesPerBuffer * m_channelCount* m_SizePerFrame);
		
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
	std::string str;
	str.resize(BlockSize + 100);
	char* buf = (char*)(str.c_str());
	
	
	
	int ms1 = 100;
	int BlockSize1 = m_sampleRate * ms1 / 1000 * m_SizePerFrame;
	std::vector<uint8_t>  v1;
	v1.resize(BlockSize1+100);
	char* buf1 = (char*)(v1.data());
	int  buf1Pos = 0;
	
	while (true) {
		m_ring.pop(buf, BlockSize);
		int state = m_vad.check((int16_t*)buf, m_sampleRate, 480);
		if (1 == state) {
			memcpy((char*)buf1 + buf1Pos, buf, BlockSize);
			buf1Pos += BlockSize;
			if (buf1Pos + BlockSize >= BlockSize1) {
				v1.resize(buf1Pos);
				OnData(v1);
				buf1Pos = 0;
			}
			//printf(".");
		}
		else if (0 == state) {
			memcpy((char*)buf1 + buf1Pos, buf, BlockSize);
			buf1Pos += BlockSize;
			if (buf1Pos > 0) {
				v1.resize(buf1Pos);
				OnData(v1);
				buf1Pos = 0;
			}
			//printf(" ");
		}
		else {
			printf("x");
		}
	}
}

void CAudioRecorderWithVad::OnData(std::vector<uint8_t>& str) {

}