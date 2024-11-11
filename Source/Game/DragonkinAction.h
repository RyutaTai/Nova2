#pragma once

#include "../Nova/AI/ActionBase.h"
#include "Dragonkin.h"

//	待機行動
namespace DragonkinAction
{
	class IdleAction : public ActionBase<Dragonkin>
	{
	public:
		IdleAction(Dragonkin* owner) :ActionBase(owner) {}
		ActionBase<Dragonkin>::State Run(const float& elapsedTime)override;
	};
}

//	通常攻撃
namespace DragonkinAction
{
	class NormalAction : public ActionBase<Dragonkin>
	{
	public:
		NormalAction(Dragonkin* owner) :ActionBase(owner) {}
		ActionBase<Dragonkin>::State Run(const float& elapsedTime)override;
	};
}

//	スキル攻撃行動
namespace DragonkinAction
{
	class SkillAction : public ActionBase<Dragonkin>
	{
	public:
		SkillAction(Dragonkin* owner) :ActionBase(owner) {}
		ActionBase<Dragonkin>::State Run(const float& elapsedTime)override;
	};
}

//	追跡行動
namespace DragonkinAction
{
	class PursuitAction : public ActionBase<Dragonkin>
	{
	public:
		PursuitAction(Dragonkin* enemy) :ActionBase(enemy) {}
		ActionBase<Dragonkin>::State Run(const float& elapsedTime)override;
	};
}

//	徘徊行動
namespace DragonkinAction
{
	class WanderAction : public ActionBase<Dragonkin>
	{
	public:
		WanderAction(Dragonkin* owner) :ActionBase(owner) {}
		ActionBase<Dragonkin>::State Run(const float& elapsedTime)override;
	};
}

//	逃走行動
namespace DragonkinAction
{
	class LeaveAction : public ActionBase<Dragonkin>
	{
	public:
		LeaveAction(Dragonkin* owner) :ActionBase(owner) {}
		ActionBase<Dragonkin>::State Run(const float& elapsedTime)override;
	};
}

//	回復行動
namespace DragonkinAction
{
	class RecoverAction : public ActionBase<Dragonkin>
	{
	public:
		RecoverAction(Dragonkin* owner) :ActionBase(owner) {}
		ActionBase<Dragonkin>::State Run(const float& elapsedTime)override;
	};
}

