#include "CYyAudioRecorder.h"
CYyAudioRecorder::CYyAudioRecorder(unsigned int BuffSize):
	m_ring(BuffSize)
{

}

int CYyAudioRecorder::OnRecordCallback(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags) {
	int state = m_vad.check((int16_t*)inputBuffer, m_sampleRate, 480);
	if (1 == state) {
		printf(".");
	}
	else if (0 == state) {
		printf(" ");
	}
	else {
		printf("x");
	}
	/*
	m_ring.push((char*)inputBuffer, 
		framesPerBuffer * m_channelCount* m_SizePerFrame);
		*/
	return paContinue;
}
void CYyAudioRecorder::process() {
	int  offset = 0;
	char buf[480*4+100];
	while (true) {
		m_ring.copy((char*)buf, 480 * m_SizePerFrame, offset);
		int state = m_vad.check((int16_t*)buf, m_sampleRate, 480);
		if (1 == state) {
			offset += 480 * m_SizePerFrame;
			printf(".");
		}
		else if (0 == state) {
			printf(" ");
			offset += 480 * m_SizePerFrame;
			/*
			//将这之前的语音转文字
			while (offset > 0) {
				int count = sizeof(buf);
				if (count > offset) { count = offset; };
				m_ring.pop((char*)buf, count);
				offset -= count;
			}
			m_ring.pop((char*)buf, 480 * m_SizePerFrame);
			offset = 0;
			*/
			continue;
		}
		else {
			printf("x");
		}
	}
}