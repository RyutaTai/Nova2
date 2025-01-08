#pragma once

#include "UI.h"
#include "../../Game/Rhythm.h"

//	”»’è•¶ŽšUI
class UIRhythmJudgment :public UI
{
public:
	UIRhythmJudgment();
	~UIRhythmJudgment() {}

	void Initialize()override;
	void Update(const float& elapsedTime)override;
	void Render()override;
	void DrawDebug()override;

	void RenderSetting(const Rhythm::JudgmentType& type);

	void SetIsVisible(const bool& isVisible);

private:
	std::unique_ptr<Sprite> judgmentText_;	//	”»’è•¶Žš

	float displayDuration_ = 0.5f;		//	”»’è•¶Žš‚ð•`‰æ‚·‚éŽžŠÔ						
	float elapsedDisplayTime_ = 0.0f;	//	”»’è•¶Žš‚ð•\Ž¦‚µ‚ÄŒo‰ß‚µ‚½ŽžŠÔ

	//Rhythm::JudgmentType currentJudgementType_ = {};

};

