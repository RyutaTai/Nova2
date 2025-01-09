#pragma once

#include "../../Nova/Resources/Sprite.h"

class UI
{
public:
	UI();
	~UI(){}

	virtual void Initialize();
	virtual void Update(const float& elapsedTime);
	virtual void Render();
	virtual void DrawDebug();

	virtual void SetIsVisible(const bool& isVisible) = 0;
	bool GetIsVisible() { return isVisible_; }

protected:
	bool isVisible_ = true;	//	UI•\Ž¦ƒtƒ‰ƒO

};

