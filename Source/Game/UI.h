#pragma once

#include "../Nova/Resources/Sprite.h"

class UI
{
public:
	UI(const std::string& filename);
	~UI(){}

	void Initialize();
	virtual void Update(const float& elapsedTime);
	void Render();

	void DrawDebug();

	void SetName(const std::string& name)		{ name_ = name; }
	void SetRenderFlag(const bool& renderFlag)	{ renderFlag_ = renderFlag; }

	const std::string			GetName()const	{ return name_; }
	Sprite::SpriteTransform*	GetTransform()	{ return spriteResource_->GetTransform(); }
	const bool					GetRenderFlag()	{ return renderFlag_; }

public:
	// TODO:Œã‚Å•ÏX‚·‚é
	virtual void SetOldHealth(const float& health){}

protected:
	std::shared_ptr<Sprite> spriteResource_ = nullptr;

private:
	std::string name_		= {};		//	UI–¼
	bool		renderFlag_ = true;		//	•`‰æƒtƒ‰ƒO

};

