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
private:
	static void ThreadFun(void* pThis);
};

