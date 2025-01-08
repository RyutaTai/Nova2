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

	void SetIsVisible(const bool& isVisible) { isVisible_ = isVisible; }
	bool GetIsVisible() { return isVisible_; }

protected:
	bool isVisible_ = true;	//	UI•\Ž¦ƒtƒ‰ƒO

};

