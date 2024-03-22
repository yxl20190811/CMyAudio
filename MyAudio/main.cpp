// MyAudio.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "CAudioRecorderWithVad.h"
#include <Windows.h>

int main()
{
    CAudioRecorderWithVad rec;
    rec.startRecording(16000, paInt32,1);
    Sleep(1000 * 3600 * 24);
    std::cout << "Hello World!\n";
}