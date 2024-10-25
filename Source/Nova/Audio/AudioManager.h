#pragma once

#include <xaudio2.h>

#include "AudioSource3D.h"
#include "AudioSource.h"

//	オーディオをまとめて管理する
class AudioManager
{
private:
	AudioManager();
	~AudioManager();

public:
	static AudioManager& Instance();

	void Update(const float& elapsedTime);	//	更新処理

	//	オーディオソース読み込み
	AudioSource* LoadAudioSource(const char* filename);
	AudioSource3D* LoadAudioSource3D(const char* filename, SoundEmitter* emitter);

	void Register(AudioSource* audio);		//	オーディオ登録
	void Clear();							//	オーディオ全削除
	void Remove(AudioSource* audio);		//	オーディオ削除

	void DrawDebug();						//	デバッグ描画

	IXAudio2*									GetXAudio()					{ return xaudio_; }
	IXAudio2MasteringVoice*						GetMasteringVoice()			{ return masteringVoice_; }
	DWORD										GetCannelmask() const		{ return channelMask_; }
	std::shared_ptr<AudioSource>				GetAudioResource(int index)	{ return audioResources_.at(index); }
	std::shared_ptr<AudioSource>				GetAudioResource(const std::string& name);
	std::vector<std::shared_ptr<AudioSource>>	GetAudioResources()			{ return audioResources_; }

private:
	static AudioManager* instance_;

	DWORD					channelMask_ = {};
	IXAudio2*				xaudio_ = nullptr;
	IXAudio2MasteringVoice* masteringVoice_ = nullptr;

	std::vector<std::shared_ptr<AudioSource>> audioResources_ = { nullptr };

};

