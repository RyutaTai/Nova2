#pragma once
#if 0


class Enemy;

//	Às”»’è
class JudgmentBase
{
public:
	JudgmentBase(Enemy* enemy) :owner(enemy) {}
	virtual bool Judgment() = 0;

protected:
	Enemy* owner;

};
#endif