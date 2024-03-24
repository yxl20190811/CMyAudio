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

    DWORD count = ::GetEnvironmentVariableA("BAIDU_APP_ID", m_APP_ID, 1000);
    if (count <= 0) {
        ::MessageBoxA(NULL, "获取BAIDU_APP_ID失败，GetEnvironmentVariable(\"BAIDU_APP_ID\")", "错误", MB_ABORTRETRYIGNORE);
        abort();
    }
    m_APP_ID[count] = 0;
    count = ::GetEnvironmentVariableA("BAIDU_APP_KEY", m_APP_KEY, 1000);
    if (count <= 0) {
        ::MessageBoxA(NULL, "获取BAIDU_APP_KEY失败，GetEnvironmentVariable(\"BAIDU_APP_KEY\")", "错误", MB_ABORTRETRYIGNORE);
        abort();
    }
    m_APP_KEY[count] = 0;
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
std::string GetString(const Value& data, const char* name) {
    if (!data.HasMember(name)) {
        return "";
    }
    return data[name].GetString();
}
void TBaiduAudio::handle(std::string s) {
    //printf("%s\n", s.c_str());
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
            const Value& type = result["type"];
            
            if (type == "MID") {
                std::string res;
                LocalTextFromUtf8(GetString(result, "asr"), res);
                printf("%s\n", res.c_str());
            }
            else if (type == "FIN") {
                std::string res;
                LocalTextFromUtf8(GetString(result, "sentence"), res);
                std::string trans = GetString(result, "sentence_trans");
                std::string x = trans + "\n" + res;
                printf("%s\n", x.c_str());
            }
            
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


void TBaiduAudio::sendFirstFrame()
{
    Document doc;
    doc.SetObject();
    doc.AddMember("type", "START", doc.GetAllocator());
    doc.AddMember("from", "zh", doc.GetAllocator());
    doc.AddMember("to", "en", doc.GetAllocator());
    doc.AddMember("sampling_rate", 16000, doc.GetAllocator());

    rapidjson::GenericStringRef<char>  appId((const char*)m_APP_ID);
    rapidjson::GenericStringRef<char>  appKey((const char*)m_APP_KEY);
    doc.AddMember("app_id", appId, doc.GetAllocator());
    doc.AddMember("app_key", appKey, doc.GetAllocator());
    doc.AddMember("return_target_tts", "false", doc.GetAllocator());
    doc.AddMember("tts_speaker", "man", doc.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    ((WebSocket::pointer)m_webSocket)->send(buffer.GetString());
}
void TBaiduAudio::SendFinalFrame()
{
    Document doc;
    doc.SetObject();
    doc.AddMember("type", "FINISH", doc.GetAllocator());
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    ((WebSocket::pointer)m_webSocket)->send(buffer.GetString());
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
    sendFirstFrame();
    ::_beginthread(ThreadFun, 0, this);
    /*
    while (!m_BaiduRetSTA) {
        Sleep(100);
    }
    */
}

void TBaiduAudio::ThreadFun(void* pThis) {
    ((TBaiduAudio*)pThis)->WebSocketPoll();
    ((TBaiduAudio*)pThis)->SendFinalFrame();
}