#include "WaveRead.h"
#include "../Others/Misc.h"

#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'


WaveReader::WaveReader(const char* filename) : strFileName(filename)
{
    HRESULT hr = FileCreate();

   
    //check the file type, should be fourccWAVE or 'XWMA'
    hr = FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    hr = ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);

    hr = FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    hr = FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    pDataBuffer = new BYTE[dwChunkSize];
    hr = ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    switch (wfx.wFormatTag)
    {
    case WAVE_FORMAT_PCM:
    case WAVE_FORMAT_IEEE_FLOAT:
        wfx.cbSize = 0;
        break;

    case WAVE_FORMAT_EXTENSIBLE:
        wfx.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
        break;

    default:
        wfx.cbSize = sizeof(WAVEFORMATEX);
        break;
    }

    length_float = (float)dwChunkSize / (wfx.nSamplesPerSec * 4.0f);
    length = dwChunkSize / (wfx.nSamplesPerSec * 4);

}

HRESULT WaveReader::FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK)
    {
        DWORD dwRead;
        if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        switch (dwChunkType)
        {
        case fourccRIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());
            break;

        default:
            if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                return HRESULT_FROM_WIN32(GetLastError());
        }

        dwOffset += sizeof(DWORD) * 2;

        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;

        if (bytesRead >= dwRIFFDataSize) return S_FALSE;

    }

    return S_OK;

}

HRESULT WaveReader::ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());
    DWORD dwRead;
    if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
        hr = HRESULT_FROM_WIN32(GetLastError());
    return hr;
}


HRESULT WaveReader::FileCreate()
{
    hFile = CreateFileA(
        strFileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (INVALID_HANDLE_VALUE == hFile)
        return HRESULT_FROM_WIN32(GetLastError());

    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

}