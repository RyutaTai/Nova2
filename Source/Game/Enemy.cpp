#include "Enemy.h"

#include "../Nova/Graphics/Graphics.h"
#include "../Nova/Others/MathHelper.h"
#include "Player.h"
#include "EnemyManager.h"

//	コンストラクタ
Enemy::Enemy(const std::string& fileName, const std::string& rootNodeName)
	:Character(fileName, rootNodeName)
{
	//	エネミーマネージャーに登録
	EnemyManager::Instance().Register(this);

}

//	ターゲット位置をランダム設定
void Enemy::SetRandomTargetPosition()
{
	float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
	float range = Mathf::RandomRange(0.0f, territoryRange_);
	targetPosition_.x = territoryOrigin_.x + sinf(theta) * range;
	targetPosition_.y = territoryOrigin_.y;
	targetPosition_.z = territoryOrigin_.z + cosf(theta) * range;
}

//	プレイヤー索敵
bool Enemy::SearchPlayer()
{
	//	プレイヤーとの高低差を考慮して3Dで距離判定をする
	const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetTransform()->GetPosition();
	const DirectX::XMFLOAT3& enemyPosition = this->GetTransform()->GetPosition();
	float vx = playerPosition.x - enemyPosition.x;
	float vy = playerPosition.y - enemyPosition.y;
	float vz = playerPosition.z - enemyPosition.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	if (dist < searchRange_)
	{
		float distXZ = sqrtf(vx * vx + vz * vz);
		//	単位ベクトル化
		vx /= distXZ;
		vz /= distXZ;

		//	方向ベクトル化
		float angleY = GetTransform()->GetRotationY();
		float frontX = sinf(angleY);
		float frontZ = cosf(angleY);
		//	2つのベクトルの内積値で前後判定
		float dot = (frontX * vx) + (frontZ * vz);
		if (dot > 0.0f)
		{
			return true;
		}
	}
	return false;
}

//	破棄
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}

