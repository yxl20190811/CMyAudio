// MyAudio.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "CAudioRecorderWithVad.h"
#include <Windows.h>
#include "TAudioRec2Baidu.h"


int main()
{
    TAudioRec2Baidu rec;
    rec.start(16000, paInt16,1);
    Sleep(1000 * 3600 * 24);
    std::cout << "Hello World!\n";
}