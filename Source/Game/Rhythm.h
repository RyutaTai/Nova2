#pragma once

#include "../Nova/Resources/Midi.h"

class Rhythm
{
public:
	Rhythm() {}
	~Rhythm() {}

	void Initialize();
	void Update(const float& elapsedTime);
	void DrawDebug();

	static Rhythm& Instance()
	{
		static Rhythm rhythm;
		return rhythm;
	}

	void	SetBPM(const float& bpm){ bpm_ = bpm; }
	float	GetBPM()				{ return bpm_; }

private:
	//	判定の種類
	enum class JudgmentType
	{
		Perfect = 0,
		Good,
		Miss,
		Max
	};

private:
	float bpm_ = 120;

	std::unique_ptr<Midi> midi_ = nullptr;	//	タイミング判定用midi(4つ打ち)

};

