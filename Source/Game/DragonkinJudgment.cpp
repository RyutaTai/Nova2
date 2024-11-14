#include "DragonkinJudgment.h"

//	IdleJudgment
namespace DragonkinJudgment
{
	bool IdleJudgment::Judgment()
	{
		//	プレイヤーを見つけていなかったら待機
		if (owner_->SearchPlayer() == false)return true;

		return false;
	}
}

//	BattleJudgment
namespace DragonkinJudgment
{
	bool BattleJudgment::Judgment()
	{
		
		return true;
	}
}

//	AttackPunchJudgment
namespace DragonkinJudgment
{
	bool AttackPunchJudgment::Judgment()
	{

		return true;
	}
}

//	AttackKickJudgment
namespace DragonkinJudgment
{
	bool AttackKickJudgment::Judgment()
	{

		return true;
	}
}

//	SkillJudgment
namespace DragonkinJudgment
{
	bool SkillJudgment::Judgment()
	{

		return false;
	}
}

//	WanderJudgment
namespace DragonkinJudgment
{
	bool WanderJudgment::Judgment()
	{

		return false;
	}
}

//	EscapeJudgment
namespace DragonkinJudgment
{
	bool EscapeJudgment::Judgment()
	{

		return false;
	}
}