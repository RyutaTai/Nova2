#pragma once

#include "UI.h"

class UITempo : public UI
{
public:
	UITempo();
	~UITempo() {}

	void Initialize()override;
	void Update(const float& elapsedTime)override;
	void Render()override;
	void DrawDebug()override;

	void UpdateDrawFlag();

	void SetBPM(const int& bpm) { bpm_ = bpm; }
	int GetBPM() { return bpm_; }

private:
	std::unique_ptr<Sprite> center_;			//	テンポガイドの中心
	std::unique_ptr<Sprite> leftSemicircle_;	//	左半円
	std::unique_ptr<Sprite> rightSemicircle_;	//	右半円

	int bpm_ = 120.0f;	//	仮でここに書いてるけど、オーディオテーブルみたいなのを用意してそこから持ってくるようにする

};

