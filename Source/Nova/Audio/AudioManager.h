#pragma once

#include <xaudio2.h>
#include <set>

#include "AudioSource3D.h"
#include "AudioSource.h"

//	オーディオをまとめて管理する
class AudioManager
{
private:
	AudioManager() {}
	~AudioManager();

public:
	static AudioManager& Instance()
	{
		static AudioManager instance;
		return instance;
	}

	void Initialize();						//	初期化処理
	void Update(const float& elapsedTime);	//	更新処理

	//	オーディオソース読み込み
	AudioSource*	LoadAudioSource(const char* filename);
	AudioSource3D*	LoadAudioSource3D(const char* filename, SoundEmitter* emitter);

	void Clear();							//	全削除
	void Register(AudioSource* audio);		//	オーディオ登録
	void Finalize();						//	オーディオ終了化


	void DrawDebug();						//	デバッグ描画

	IXAudio2*									GetXAudio()							{ return xaudio_; }
	IXAudio2MasteringVoice*						GetMasteringVoice()					{ return masteringVoice_; }
	DWORD										GetCannelmask() const				{ return channelMask_; }
	AudioSource*								GetAudioResource(const int& index)	{ return audioResources_.at(index); }
	AudioSource*								GetAudioResource(const std::string& name);
	std::vector<AudioSource*>					GetAudioResources()					{ return audioResources_; }
	void Remove(AudioSource* audio);			//	オーディオ削除
	void RemoveByScene(const std::string& sceneName);

private:
	DWORD					channelMask_ = {};
	IXAudio2*				xaudio_			= nullptr;
	IXAudio2MasteringVoice* masteringVoice_ = nullptr;

	std::vector<AudioSource*>	audioResources_ = {};
	std::set<AudioSource*>		audioRemoves_ = {};


};

