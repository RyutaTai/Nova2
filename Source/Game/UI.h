#pragma once

#include "../Nova/Resources/Sprite.h"

class UI
{
public:
	UI(const std::string& fileName);
	~UI(){}

	void Initialize();
	void Update(const float& elapsedTime);
	void Render();

	void DrawDebug();

	void SetName(const std::string& name)		{ name_ = name; }
	void SetRenderFlag(const bool& renderFlag)	{ renderFlag_ = renderFlag; }

	const std::string			GetName()const	{ return name_; }
	Sprite::SpriteTransform*	GetTransform()	{ return spriteResource_.get()->GetTransform(); }
	const bool					GetRenderFlag()	{ return renderFlag_; }

private:
	std::shared_ptr<Sprite> spriteResource_ = nullptr;

	std::string name_		= {};		//	UI–¼
	bool		renderFlag_ = false;	//	•`‰æƒtƒ‰ƒO

};

