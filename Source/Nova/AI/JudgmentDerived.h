#pragma once
#if 0
#include "JudgmentBase.h"
#include "../../Game/Enemy.h"

//	BattleNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class BattleJudgment : public JudgmentBase
{
public:
	BattleJudgment(Enemy* enemy) :JudgmentBase(enemy) {};
	// ”»’è
	bool Judgment();

};

//	AttackNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class AttackJudgment : public JudgmentBase
{
public:
	AttackJudgment(Enemy* enemy) :JudgmentBase(enemy) {};
	//	”»’è
	bool Judgment();

};

//	SkillNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class SkillShotJudgment : public JudgmentBase
{
public:
	SkillShotJudgment(Enemy* enemy) :JudgmentBase(enemy) {};
	//	”»’è
	bool Judgment();

};

//	WanderNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class WanderJudgment : public JudgmentBase
{
public:
	WanderJudgment(Enemy* enemy) :JudgmentBase(enemy) {};
	//	”»’è
	bool Judgment();

};

//	Escape‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class EscapeJudgment : public JudgmentBase
{
public:
	EscapeJudgment(Enemy* enemy) :JudgmentBase(enemy) {};
	//	”»’è
	bool Judgment();
};
#endif