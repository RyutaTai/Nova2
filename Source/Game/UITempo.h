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

private:
	std::unique_ptr<Sprite> center_;				//	テンポガイドの中心
	std::unique_ptr<Sprite> leftSemicircle_;	//	左半円
	std::unique_ptr<Sprite> rightSemicircle_;	//	右半円


};

