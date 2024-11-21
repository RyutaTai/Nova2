#pragma once

#include "UI.h"

//	テンポガイドUI	真ん中の円一つ、サイドの半円4つずつ
class UITempo : public UI
{
public:
	UITempo();
	~UITempo() {}

	void Initialize()override;
	void Update(const float& elapsedTime)override;
	void Render()override;
	void DrawDebug()override;

	void UpdatePosition(const float& elapsedTime);
	void UpdateScale(const float& elapsedTime);
	void UpdateDrawFlag();

	void SetBPM(const int& bpm) { bpm_ = bpm; }
	int GetBPM() { return bpm_; }

private:
	struct Semicircle
	{
		std::unique_ptr<Sprite> left_;
		std::unique_ptr<Sprite> right_;
	};
	
	std::unique_ptr<Sprite> center_;			//	テンポガイドの中心
	//std::unique_ptr<Sprite> leftSemicircle_[4];	//	左半円
	//std::unique_ptr<Sprite> rightSemicircle_[4];	//	右半円

	std::unique_ptr<Semicircle> semicircles_[4];	//	半円の組

	int bpm_ = 120.0f;	//	仮でここに書いてるけど、オーディオテーブルみたいなのを用意してそこから持ってくるようにする

};

