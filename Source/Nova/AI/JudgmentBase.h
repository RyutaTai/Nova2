#pragma once
#if 0


class Enemy;

//	���s����
class JudgmentBase
{
public:
	JudgmentBase(Enemy* enemy) :owner(enemy) {}
	virtual bool Judgment() = 0;

protected:
	Enemy* owner;

};
#endif