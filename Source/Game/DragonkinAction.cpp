#include "DragonkinAction.h"

//	待機行動
namespace DragonkinAction
{
	ActionBase<Dragonkin>::State IdleAction::Run(const float& elapsedTime)
	{

	}
}

//	通常攻撃
namespace DragonkinAction
{
	ActionBase<Dragonkin>::State NormalAction::Run(const float& elapsedTime)
	{

	}
}

//	スキル攻撃行動
namespace DragonkinAction
{
	ActionBase<Dragonkin>::State SkillAction::Run(const float& elapsedTime)
	{

	}
}

//	追跡行動
namespace DragonkinAction
{
	ActionBase<Dragonkin>::State PursuitAction::Run(const float& elapsedTime)
	{

	}
}

//	徘徊行動
namespace DragonkinAction
{
	ActionBase<Dragonkin>::State WanderAction::Run(const float& elasedTime)
	{

	}
}

//	逃走行動
namespace DragonkinAction
{
	ActionBase<Dragonkin>::State LeaveAction::Run(const float& elapsedTime)
	{

	}
}

//	回復行動
namespace DragonkinAction
{
	ActionBase<Dragonkin>::State RecoverAction::Run(const float& elapsedTime)
	{

	}
}