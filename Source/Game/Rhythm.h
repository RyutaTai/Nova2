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
		None,			//	判定済み
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

	JudgmentType GetJudgmentType(const double& inputTime);

	void	SetBPM(const float& bpm){ bpm_ = bpm; }
	float	GetBPM()				{ return bpm_; }

	//	midiを見てノートオンならtrueを返す(テンポに合わせた動きをさせるために使用する)
	bool	GetRhythm();

private:
	float bpm_ = 140.0f;	//	楽曲のbpm

	//	後で定数にする
	//const float PerfectRange_	= 0.025f;
	//const float GoodRange_		= 0.050f;
	double debugMaxMidiTimer_ = 0.0f;
	float debugDelta_ = 0.0f;
	float debugClosestNoteTime_ = 0.0f;
	float debugInputTime_ = 0.0f;
	double PerfectRange_ = 0.09;	//	Perfectの範囲
	double GoodRange_ = 0.135;		//	Goodの範囲

	std::unique_ptr<Midi> midi_ = nullptr;	//	タイミング判定用midi(4つ打ち)

};

