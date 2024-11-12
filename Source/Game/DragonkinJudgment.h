#pragma once

#include "../Nova/AI/JudgmentBase.h"
#include "Dragonkin.h"

//	BattleNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
namespace DragonkinJudgment
{
	class BattleJudgment : public JudgmentBase
	{
	public:
		BattleJudgment(Dragonkin* owner) :JudgmentBase(owner) {};
		// ”»’è
		bool Judgment();

	};
}

//	AttackNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
namespace DragonkinJudgment
{
	class AttackJudgment : public JudgmentBase
	{
	public:
		AttackJudgment(Dragonkin* owner) :JudgmentBase(owner) {};
		//	”»’è
		bool Judgment();

	};
}

//	SkillNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
namespace DragonkinJudgment
{
	class SkillJudgment : public JudgmentBase
	{
	public:
		SkillJudgment(Dragonkin* owner) :JudgmentBase(owner) {};
		//	”»’è
		bool Judgment();

	};
}

//	WanderNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
namespace DragonkinJudgment
{
	class WanderJudgment : public JudgmentBase
	{
	public:
		WanderJudgment(Dragonkin* owner) :JudgmentBase(owner) {};
		//	”»’è
		bool Judgment();

	};
}

//	Escape‚É‘JˆÚ‚Å‚«‚é‚©”»’è
namespace DragonkinJudgment
{
	class EscapeJudgment : public JudgmentBase
	{
	public:
		EscapeJudgment(Dragonkin* owner) :JudgmentBase(owner) {};
		//	”»’è
		bool Judgment();
	};
}