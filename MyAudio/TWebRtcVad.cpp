#include "TWebRtcVad.h"
TWebRtcVad::TWebRtcVad(int WebRtcVadMode) {
	m_vadInst = WebRtcVad_Create();
	if (NULL == m_vadInst) {
		printf("WebRtcVad_Create fail\n");
		return;
	}
	int status = WebRtcVad_Init(m_vadInst);
	if (status != 0) {
		printf("WebRtcVad_Init fail\n");
		WebRtcVad_Free(m_vadInst);
		m_vadInst = NULL;
		return;
	}
	status = WebRtcVad_set_mode(m_vadInst, WebRtcVadMode);
	if (status != 0) {
		printf("WebRtcVad_set_mode fail\n");
		WebRtcVad_Free(m_vadInst);
		m_vadInst = NULL;
		return;
	}
}
TWebRtcVad::~TWebRtcVad() {
	if (NULL != m_vadInst) {
		WebRtcVad_Free(m_vadInst);
		m_vadInst = NULL;
	}
}
int TWebRtcVad::check(const int16_t* audio_frame,
	size_t sampleRate,
	size_t frame_length) {
	if (NULL == m_vadInst) {
		printf("WebRtcVad_Create fail\n");
		return false;
	}
	return WebRtcVad_Process(m_vadInst, (int)sampleRate, audio_frame, frame_length);
}