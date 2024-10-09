#pragma once

#include <xaudio2.h>

#include "AudioSource3D.h"
#include "AudioSource.h"

// オーディオ
class Audio
{
private:
	Audio();
	~Audio();

public:
	IXAudio2MasteringVoice* GetMasteringVoice() { return masteringVoice_; }
	IXAudio2* GetXAudio() { return xaudio_; }
	DWORD GetCannelmask() const { return channelMask_; }

	std::shared_ptr<WaveReader>::element_type* GetResource() { return resource_.get(); }

public:
	// インスタンス取得
	static Audio& Instance() 
	{ 
		static Audio instance;
		return instance;
	}

	// オーディオソース読み込み
	AudioSource* LoadAudioSource(const char* filename);
	AudioSource3D* LoadAudioSource3D(const char* filename, SoundEmitter* emitter);

private:
	IXAudio2* xaudio_ = nullptr;
	IXAudio2MasteringVoice* masteringVoice_ = nullptr;
	std::shared_ptr<WaveReader> resource_;
	DWORD channelMask_{};
};

