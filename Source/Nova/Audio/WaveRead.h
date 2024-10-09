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
    HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);
    HRESULT FileCreate();


public:

    DWORD GetAudioBytes() { return dwChunkSize; }
    BYTE* GetAudioData() { return pDataBuffer; }
    WAVEFORMATEX GetWaveForMatex() { return wfx; }
    UINT32 GetPlayLength() const { return length; }
    float GetPlayLengthFLOAT() const { return length_float; }

private:


    WAVEFORMATEX wfx = { 0 };
    XAUDIO2_BUFFER buffer = { 0 };

    const char* strFileName;
    // Open the file
    HANDLE hFile;

    DWORD dwChunkSize; //size of the audio buffer in bytes
    DWORD dwChunkPosition;
   
    DWORD filetype;
   
    BYTE* pDataBuffer; //buffer containing audio data
    UINT32 length;
    float length_float;
};