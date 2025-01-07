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

	void SetDrawFlag(const bool& drawFlag) { drawFlag_ = drawFlag; }
	bool GetDrawFlag() { return drawFlag_; }

private:
	bool drawFlag_ = true;	//	•`‰æƒtƒ‰ƒO

};

