#pragma once

#include "../Nova/Resources/Sprite.h"

class UI
{
public:
	UI(const wchar_t* filename);
	~UI(){}

	virtual void Initialize();
	virtual void Update(const float& elapsedTime);
	virtual void Render();

	virtual void DrawDebug();

	void SetName(const std::string& name)		{ name_ = name; }
	void SetRenderFlag(const bool& renderFlag)	{ renderFlag_ = renderFlag; }

	const std::string			GetName()const	{ return name_; }
	Sprite::SpriteTransform*	GetTransform()	{ return sprite_->GetTransform(); }
	const bool					GetRenderFlag()	{ return renderFlag_; }


protected:
	std::unique_ptr<Sprite> sprite_;

private:
	std::string name_		= {};		//	UI–¼
	bool		renderFlag_ = true;		//	•`‰æƒtƒ‰ƒO

};

