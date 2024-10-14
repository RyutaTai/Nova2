#pragma once
#include <windows.h>
#include <xaudio2.h>
#include <tchar.h>
class WaveReader
{
public:

    WaveReader(const char* filename);
    ~WaveReader() {}

private:

    HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);
    HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD bufferSize, DWORD bufferOffset);
    HRESULT FileCreate();


public:

    DWORD GetAudioBytes() { return dwChunkSize_; }
    BYTE* GetAudioData() { return pDataBuffer_; }
    WAVEFORMATEX GetWaveForMatex() { return wfx_; }
    UINT32 GetPlayLength() const { return length_; }
    float GetPlayLengthFLOAT() const { return lengthFloat_; }

private:


    WAVEFORMATEX wfx_ = { 0 };
    XAUDIO2_BUFFER buffer_ = { 0 };

    const char* strFilename_;
    // Open the file
    HANDLE hFile_;

    DWORD dwChunkSize_; //size of the audio buffer in bytes
    DWORD dwChunkPosition_;
   
    DWORD filetype_;
   
    BYTE* pDataBuffer_; //buffer containing audio data
    UINT32 length_;
    float lengthFloat_;
};