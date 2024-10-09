#pragma once

#include "Enemy.h"

class Dragonkin : public Enemy
{
public:
	Dragonkin();
	~Dragonkin()override {}

	void Initialize()override;
	void Update(const float& elapsedTime)		override;
	void Render()		override;

	bool RayVsVertical(const float& elapsedTime)override;
	bool RayVsHorizontal(const float& elapsedTime)override;
	void Move(const float& elapsedTime)override {}

	void DrawDebug()	override;
	void DrawDebugPrimitive()override;

private:
	//	アニメーション情報(Boss)
	enum DragonkinAnimation
	{
		ANIM_ATTACK01 = 0, 	//	
		ANIM_ATTACK02,
		ANIM_ATTACK03,
		ANIM_ATTACK04,
		ANIM_INIT01,
		ANIM_INIT02,
		ANIM_INIT03,
		ANIM_RUN,
		ANIM_SKIL01,
		ANIM_SKIL02,
		ANIM_SKIL03,
		ANIM_SKIL04,
		ANIM_SKIL05,
		ANIM_SKIL06,
		ANIM_SKIL07,
		ANIM_SKIL08,
		ANIM_IDLE01,		//	待機
		ANIM_IDLE02,		//	待機2
		ANIM_WALK,
		ANIM_DMG_DIE01,
		ANIM_DMG_DIE02,
		ANIM_DMG_DIE03,
		ANIM_DMG_DIE_DOWN,
		ANIM_DMG_DOWN01,
		ANIM_DMG_DOWN02,
		ANIM_DMG_HIT01,
		ANIM_DMG_HIT02,
		ANIM_STU01,
		ANIM_STU02,
		ANIM_STU03,
		ANIM_BD01,
		ANIM_BD02,
		ANIM_BD03,
		ANIM_BD04,
		ANIM_BD05,
		ANIM_CINE02,
		ANIM_CINE03,
		MAX,
	};
};

