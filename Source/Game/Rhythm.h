#pragma once

#include "../Nova/Audio/Midi.h"

class Rhythm
{
public:
	//	判定の種類
	enum class JudgmentType
	{
		Perfect = 0,
		Good,
		Miss,
		Max
	};

public:
	Rhythm() {}
	~Rhythm() {}

	void Initialize();
	void Update();
	void DrawDebug();

	static Rhythm& Instance()
	{
		static Rhythm rhythm;
		return rhythm;
	}
	
	const double GetCurrentMidiTime()const { return midi_->GetCurrentTimer(); }

	void GetJudgmentType(const double& inputTime, const double& elapsedTime);

	void	SetBPM(const float& bpm){ bpm_ = bpm; }
	float	GetBPM()				{ return bpm_; }

private:
	float bpm_ = 140.0f;	//	楽曲のbpm

	//	後で定数にする
	//const float PerfectRange_	= 0.025f;
	//const float GoodRange_		= 0.050f;
	double debugMaxMidiTimer_ = 0.0f;
	float debugDelta_ = 0.0f;
	float debugClosestNoteTime_ = 0.0f;
	float debugInputTime_ = 0.0f;
	float PerfectRange_ = 0.032f;	//	約2フレーム
	float GoodRange_ = 0.096f;		//	約6フレーム

	std::unique_ptr<Midi> midi_ = nullptr;	//	タイミング判定用midi(4つ打ち)

};

