#pragma once

#include "Enemy.h"
#include "../Nova/AI/BehaviorTree.h"
#include "../Nova/AI/NodeBase.h"

class BehaviorTree;
class BehaviorData;
class NodeBase;

class Dragonkin : public Enemy
{
public:
	Dragonkin();
	~Dragonkin()override {}

public:
	//	アニメーション情報(Boss)
	enum class AnimationType
	{
		AttackPunch = 0, 	//	
		AttackKick,
		AttackWing,
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
		Idle01,		//	待機
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
		Max,
	};

	//	ステートの種類
	enum class StateType
	{
		Idle = 0, 		//	待機
		Move,			//	移動
		Attack,			//	攻撃
		ComboOne1,		//	コンボ0_1
		ComboOne2,		//	コンボ0_2
		ComboOne3,		//	コンボ0_3
		ComboOne4,		//	コンボ0_4
		ComboOne5,		//	コンボ0_5
		ComboOne6,		//	コンボ0_6
		ComboOne7,		//	コンボ0_7
		Dodge,			//	回避
		Max,			//	ステート最大数
	};

public:
	void Initialize()override;
	void Update(const float& elapsedTime)override;
	void Render()override;

	bool RayVsVertical(const float& elapsedTime)override;
	bool RayVsHorizontal(const float& elapsedTime)override;
	void Move(const float& elapsedTime)override {}
	void Destroy()override;

	void DrawDebug()override;
	void DrawDebugPrimitive()override;

	void PlayAnimation(const AnimationType& animType, const bool& loop = false, const float& blendTime = 1.0f, const float& startFrame = 0.0f, const float& animSpeed = 1.0f);

private:
	void UpdateBehaviorTree(const float& elapsedTime);	//	ビヘイビアツリー更新処理

private:
	static const int MAX_HP = 70;

	BehaviorTree*	behaviorTree_	= nullptr;
	BehaviorData*	behaviorData_	= nullptr;
	NodeBase*		activeNode_		= nullptr;


};

