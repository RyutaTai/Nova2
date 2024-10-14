#pragma once

#include "Audio.h"

//	オーディオをまとめて管理する
class AudioManager
{
private:
	AudioManager() {}
	~AudioManager() {}

public:
	static AudioManager& Instance()
	{
		static AudioManager instance;
		return instance;
	}

	void Update(const float& elapsedTime);

	void Register(AudioSource* audio);	//	オーディオ登録
	void Clear();						//	オーディオ全削除
	void Remove(AudioSource* audio);	//	オーディオ削除

	void DrawDebug();

	AudioSource*				GetAudio(int index) { return audioResources_.at(index); }
	std::vector<AudioSource*>	GetAudios()			{ return audioResources_; }
	int							GetAudioCount()		{ return static_cast<int>(audioResources_.size()); }

private:
	std::vector<AudioSource*> audioResources_ = {};	//	オーディオ

};

