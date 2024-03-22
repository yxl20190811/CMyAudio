#pragma once
#include <iostream>  
#include <fstream>  
#include <portaudio.h> 

class CAudioRecorder
{
public:
    CAudioRecorder();
    ~CAudioRecorder();
    bool startRecording(
        unsigned int sampleRate = 16000, 
        PaSampleFormat sampleFormat = paFloat32,
        unsigned int channelCount = 1);
    bool stopRecording();
protected:
    unsigned int m_sampleRate;
    unsigned int m_channelCount;
    PaSampleFormat m_sampleFormat;
    unsigned int m_SizePerFrame;
    PaStream* m_stream;
private:
    void initializePortAudio();
    void terminatePortAudio();
    void openStream();
    void closeStream();
private:
    static int RecordCallback(const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData);
protected:
    virtual int OnRecordCallback(const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags);
};

