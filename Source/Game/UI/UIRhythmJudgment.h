#pragma once

#include "UI.h"

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

private:
	std::unique_ptr<Sprite> judgmentText_;	//	”»’è•¶Žš

};

