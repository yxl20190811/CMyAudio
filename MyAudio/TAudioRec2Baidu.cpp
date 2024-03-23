#include "TAudioRec2Baidu.h"

void TAudioRec2Baidu::OnData(std::vector<uint8_t>& str) {
	TBaiduAudio::sendBinData(str);
}
void TAudioRec2Baidu::start(unsigned int sampleRate,
	PaSampleFormat sampleFormat,
	unsigned int channelCount
) {
	TBaiduAudio::init();
	CAudioRecorderWithVad::startRecording(sampleRate, sampleFormat, channelCount);
}