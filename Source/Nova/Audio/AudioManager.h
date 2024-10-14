#pragma once

#include "Audio.h"

//	�I�[�f�B�I���܂Ƃ߂ĊǗ�����
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

	void Register(AudioSource* audio);	//	�I�[�f�B�I�o�^
	void Clear();						//	�I�[�f�B�I�S�폜
	void Remove(AudioSource* audio);	//	�I�[�f�B�I�폜

	void DrawDebug();

	AudioSource*				GetAudio(int index) { return audioResources_.at(index); }
	std::vector<AudioSource*>	GetAudios()			{ return audioResources_; }
	int							GetAudioCount()		{ return static_cast<int>(audioResources_.size()); }

private:
	std::vector<AudioSource*> audioResources_ = {};	//	�I�[�f�B�I

};

