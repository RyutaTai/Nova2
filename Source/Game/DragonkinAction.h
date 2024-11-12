#pragma once

#include "../Nova/AI/ActionBase.h"
#include "Dragonkin.h"

//	待機行動
namespace DragonkinAction
{
	class IdleAction : public ActionBase
	{
	public:
		IdleAction(Dragonkin* owner) :ActionBase(owner) {}
		ActionBase::State Run(const float& elapsedTime)override;
		void DrawDebug()override;
	};
}

//	通常攻撃
namespace DragonkinAction
{
	class NormalAction : public ActionBase
	{
	public:
		NormalAction(Dragonkin* owner) :ActionBase(owner) {}
		ActionBase::State Run(const float& elapsedTime)override;
		void DrawDebug()override;
	};
}

//	スキル攻撃行動
namespace DragonkinAction
{
	class SkillAction : public ActionBase
	{
	public:
		SkillAction(Dragonkin* owner) :ActionBase(owner) {}
		ActionBase::State Run(const float& elapsedTime)override;
		void DrawDebug()override;
	};
}

//	追跡行動
namespace DragonkinAction
{
	class PursuitAction : public ActionBase
	{
	public:
		PursuitAction(Dragonkin* enemy) :ActionBase(enemy) {}
		ActionBase::State Run(const float& elapsedTime)override;
		void DrawDebug()override;
	};
}

//	徘徊行動
namespace DragonkinAction
{
	class WanderAction : public ActionBase
	{
	public:
		WanderAction(Dragonkin* owner) :ActionBase(owner) {}
		ActionBase::State Run(const float& elapsedTime)override;
		void DrawDebug()override;
	};
}

//	逃走行動
namespace DragonkinAction
{
	class LeaveAction : public ActionBase
	{
	public:
		LeaveAction(Dragonkin* owner) :ActionBase(owner) {}
		ActionBase::State Run(const float& elapsedTime)override;
		void DrawDebug()override;
	};
}

//	回復行動
namespace DragonkinAction
{
	class RecoverAction : public ActionBase
	{
	public:
		RecoverAction(Dragonkin* owner) :ActionBase(owner) {}
		ActionBase::State Run(const float& elapsedTime)override;
		void DrawDebug()override;
	};
}

