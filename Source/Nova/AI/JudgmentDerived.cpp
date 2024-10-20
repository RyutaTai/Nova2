#if 0
#include "JudgmentDerived.h"

#include "Player.h"
#include "Mathf.h"

//	BattleNodeに遷移できるか判定
bool BattleJudgment::Judgment()
{
	//	プレイヤーが見つかるか
	if (owner->SearchPlayer())
	{
		return true;
	}
	return false;
}

//	AttackNodeに遷移できるか判定
bool AttackJudgment::Judgment()
{
	//	対象との距離を算出
	DirectX::XMFLOAT3 position = owner->GetPosition();
	DirectX::XMFLOAT3 targetPosition = Player::Instance().GetPosition();

	float vx = targetPosition.x - position.x;
	float vy = targetPosition.y - position.y;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	if (dist < owner->GetAttackRange())
	{
		//	AttackNodeへ遷移できる
		return true;
	}
	return false;
}

//	SkillNodeに遷移できるか判定
bool SkillShotJudgment::Judgment()
{
	//	HPが80%未満のとき、Skill発動可能
	int health = owner->GetHealth();
	int baseHealth = static_cast<int>(owner->GetMaxHealth() * 0.8);
	if (health < baseHealth)
	{
		//	SkillNodeへ遷移できる
		return true;
	}
	return false;
}

//	WanderNodeに遷移できるか判定
bool WanderJudgment::Judgment()
{
	//	目的地点までのXZ平面での距離判定
	DirectX::XMFLOAT3 position = owner->GetPosition();
	DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;

	//	目的地から離れている場合
	float radius = owner->GetRadius();
	if (distSq > radius * radius)
	{
		return true;
	}
	return false;
}

//	EscapeNodeに遷移できるか判定
bool EscapeJudgment::Judgment()
{
	//	HPが半分以下のとき
	if (owner->GetHealth() <= (owner->GetMaxHealth() * 0.5))
	{
		return true;
	}
	return false;
}
#endif