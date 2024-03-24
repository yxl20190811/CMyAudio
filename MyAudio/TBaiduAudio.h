#pragma once
#include <string>
#include <vector>
#include <mutex> 

class TBaiduAudio
{
public:
	TBaiduAudio();
	~TBaiduAudio();
	void init();
	void sendBinData(std::vector<uint8_t>& str);
private:
	static void handle(std::string s);
	void WebSocketPoll();
	void* m_webSocket;
	bool m_BaiduRetSTA;
	std::mutex m_mtx;
	char m_APP_ID[1000];
	char m_APP_KEY[1000];
private:
	static void ThreadFun(void* pThis);
	void sendFirstFrame();
	void SendFinalFrame();
};

