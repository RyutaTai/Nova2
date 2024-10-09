#pragma once
#if  0
#include "ActionBase.h"
#include "../../Game/Enemy.h"

//	通常攻撃
class NormalAction : public ActionBase
{
public:
	NormalAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

//	スキル攻撃行動
class SkillAction : public ActionBase
{
public:
	SkillAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

//	追跡行動
class PursuitAction : public ActionBase
{
public:
	PursuitAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

//	徘徊行動
class WanderAction : public ActionBase
{
public:
	WanderAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

//	待機行動
class IdleAction : public ActionBase
{
public:
	IdleAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

//	逃走行動
class LeaveAction : public ActionBase
{
public:
	LeaveAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

//	回復行動
class RecoverAction : public ActionBase
{
public:
	RecoverAction(EnemyBlueSlime* enemy) :ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};
#endif