#include "CYyAudioRecorder.h"
CYyAudioRecorder::CYyAudioRecorder(unsigned int BuffSize):
	m_ring(BuffSize)
{

}

int CYyAudioRecorder::OnRecordCallback(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags) {
	
	m_ring.push((char*)inputBuffer, 
		framesPerBuffer * m_channelCount* m_SizePerFrame);
		
	return paContinue;
}
void CYyAudioRecorder::process() {
	int  offset = 0;
	char buf[480*4+100];
	int BlockSize = 480 * m_SizePerFrame;
	while (true) {
		m_ring.pop((char*)buf, BlockSize);
		int state = m_vad.check((int16_t*)buf, m_sampleRate, 480);
		if (1 == state) {
			printf(".");
		}
		else if (0 == state) {
			printf(" ");
		}
		else{
			printf("x");
		}
	}
}