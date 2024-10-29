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
	std::unique_ptr<Sprite> tempo_;	//	テンポサポート

};

