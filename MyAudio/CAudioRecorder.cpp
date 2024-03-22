#include "CAudioRecorder.h"
CAudioRecorder::CAudioRecorder()
{
    m_stream = nullptr;
}
CAudioRecorder::~CAudioRecorder() {
    closeStream();
    terminatePortAudio();
}

void CAudioRecorder::terminatePortAudio() {
    Pa_Terminate();
}
void CAudioRecorder::closeStream() {
    if (m_stream) {
        Pa_CloseStream(m_stream);
        m_stream = nullptr;
    }
}

bool CAudioRecorder::startRecording(
    unsigned int sampleRate, 
    PaSampleFormat sampleFormat,
    unsigned int channelCount) {
    if (nullptr != m_stream) {
        return false;
    }
    m_sampleRate = sampleRate;
    m_channelCount = channelCount;
    m_sampleFormat = sampleFormat;
    switch (sampleFormat) {
    case paFloat32:
    case paInt32:
        m_SizePerFrame = 4;
        break;
    case paInt16:
        m_SizePerFrame = 2;
        break;
    case paInt8:
    case paUInt8:
        m_SizePerFrame =1;
        break;
    default:
        abort();
    }
    
    m_stream = nullptr;
    if (Pa_Initialize() != paNoError) {
        std::cerr << "PortAudio initialization failed." << std::endl;
        exit(1);
    }
    openStream();
    return Pa_StartStream(m_stream) == paNoError;
}

bool CAudioRecorder::stopRecording() {
    return Pa_StopStream(m_stream) == paNoError;
}

void CAudioRecorder::initializePortAudio() {
    if (Pa_Initialize() != paNoError) {
        std::cerr << "PortAudio initialization failed." << std::endl;
        exit(1);
    }
}


void CAudioRecorder::openStream() {
    PaStreamParameters inputParameters;
    inputParameters.device = Pa_GetDefaultInputDevice();
    if (inputParameters.device == paNoDevice) {
        std::cerr << "No default input device available." << std::endl;
        exit(1);
    }
    inputParameters.channelCount = m_channelCount;
    inputParameters.sampleFormat = m_sampleFormat;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaError err = Pa_OpenStream(&m_stream, &inputParameters, nullptr, 
        m_sampleRate, 480, paClipOff,
        RecordCallback, this);
    if (err != paNoError) {
        std::cerr << "Error opening PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        exit(1);
    }
}

int CAudioRecorder::RecordCallback(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData) {
    return ((CAudioRecorder*)userData)->OnRecordCallback(
        inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags
    );
}
int CAudioRecorder::OnRecordCallback(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags) {
    return paContinue;
}
