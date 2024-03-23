#include "TBaiduAudio.h"
#include "easywsclient.hpp"
#include <process.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>

using namespace rapidjson;

using easywsclient::WebSocket;
TBaiduAudio::TBaiduAudio() {
	m_webSocket = NULL;
    m_BaiduRetSTA = false;
}
TBaiduAudio::~TBaiduAudio() {
	if (NULL != m_webSocket) {
		delete ((WebSocket::pointer)m_webSocket);
	}
}

void LocalTextFromUtf8(const std::string& req, std::string& res) {
    int nLen1 = MultiByteToWideChar(CP_UTF8, NULL, req.c_str(), -1, NULL, 0);
    std::string textW;
    textW.resize(2 * nLen1 + 2);
    wchar_t* wstr = (wchar_t*)(textW.c_str());
    MultiByteToWideChar(CP_UTF8, NULL, req.c_str(), -1, wstr, nLen1);
    wstr[nLen1 - 1] = 0;

    int nLen2 = WideCharToMultiByte(CP_ACP, NULL, wstr, -1, NULL, NULL, NULL, NULL);
    res.resize(nLen2);
    char* text = (char*)(res.c_str());
    WideCharToMultiByte(CP_ACP, NULL, wstr, -1, text, nLen2, NULL, NULL);
    text[nLen2] = 0;
}

void TBaiduAudio::handle(std::string s) {
    Document doc;
    doc.Parse(s.c_str());
    if (!doc.IsObject()) {
        //printf("解析从OpenAi返回的JSON失败\n");
        return;
    }
    if (doc.HasMember("data")) {
        const Value& data = doc["data"];
        std::string  tmp = data["status"].GetString();
        if (tmp == "STA") {
            //m_BaiduRetSTA = true;
            printf("STA\n");
        }
        else if (tmp == "TRN") {
            const Value& result = data["result"];
            const Value& asr = result["asr"];
            std::string ret = asr.GetString();
            std::string res;
            LocalTextFromUtf8(ret, res);
            printf("%s\n", res.c_str());
        }
    }else{
        printf("%s\n", s.c_str());
    }
	return;
}
void TBaiduAudio::WebSocketPoll() {
	WebSocket::pointer ws = (WebSocket::pointer)m_webSocket;
	while (true) {
		WebSocket::readyStateValues s = ws->getReadyState();
        if (WebSocket::readyStateValues::CLOSED == s){
            return;
        }
        std::lock_guard<std::mutex> lock(m_mtx);
		ws->poll();
		ws->dispatch(handle);
	}
}

const char* APP_ID = "54565458";
const char* APP_KEY = "i8qVpOb3YZUw97TbV7ueXB8E";

void sendFirstFrame(WebSocket::pointer ws)
{
    Document doc;
    doc.SetObject();
    doc.AddMember("type", "START", doc.GetAllocator());
    doc.AddMember("from", "zh", doc.GetAllocator());
    doc.AddMember("to", "en", doc.GetAllocator());
    doc.AddMember("sampling_rate", 16000, doc.GetAllocator());

    doc.AddMember("app_id", "54565458", doc.GetAllocator());
    doc.AddMember("app_key", "i8qVpOb3YZUw97TbV7ueXB8E", doc.GetAllocator());
    doc.AddMember("return_target_tts", "false", doc.GetAllocator());
    doc.AddMember("tts_speaker", "man", doc.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    ws->send(buffer.GetString());
}
void SendFinalFrame(WebSocket::pointer ws)
{
    Document doc;
    doc.SetObject();
    doc.AddMember("type", "FINISH", doc.GetAllocator());
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    ws->send(buffer.GetString());
};

void TBaiduAudio::sendBinData(std::vector<uint8_t>& str) {
    std::lock_guard<std::mutex> lock(m_mtx);
    ((WebSocket::pointer)m_webSocket)->sendBinary(str);
}
void TBaiduAudio::init() {
    INT rc;
    WSADATA wsaData;

    rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (rc) {
        printf("WSAStartup Failed.\n");
        return;
    }
	m_webSocket = WebSocket::from_url("ws://aip.baidubce.com/ws/realtime_speech_trans");
    if (NULL == m_webSocket) {
        abort();
    }
    sendFirstFrame((WebSocket::pointer)m_webSocket);
    ::_beginthread(ThreadFun, 0, this);
    /*
    while (!m_BaiduRetSTA) {
        Sleep(100);
    }
    */
}

void TBaiduAudio::ThreadFun(void* pThis) {
    ((TBaiduAudio*)pThis)->WebSocketPoll();
    SendFinalFrame((WebSocket::pointer)((TBaiduAudio*)pThis)->m_webSocket);
}