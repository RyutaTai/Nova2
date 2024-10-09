#include "Enemy.h"

#include "../Nova/Graphics/Graphics.h"
#include "../Nova/Others/MathHelper.h"
#include "Player.h"
#include "EnemyManager.h"

//	�R���X�g���N�^
Enemy::Enemy(const std::string& fileName, const std::string& rootNodeName)
	:Character(fileName, rootNodeName)
{
	//	�G�l�~�[�}�l�[�W���[�ɓo�^
	EnemyManager::Instance().Register(this);

}

//	�^�[�Q�b�g�ʒu�������_���ݒ�
void Enemy::SetRandomTargetPosition()
{
	float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
	float range = Mathf::RandomRange(0.0f, territoryRange_);
	targetPosition_.x = territoryOrigin_.x + sinf(theta) * range;
	targetPosition_.y = territoryOrigin_.y;
	targetPosition_.z = territoryOrigin_.z + cosf(theta) * range;
}

//	�v���C���[���G
bool Enemy::SearchPlayer()
{
	//	�v���C���[�Ƃ̍��፷���l������3D�ŋ������������
	const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetTransform()->GetPosition();
	const DirectX::XMFLOAT3& enemyPosition = this->GetTransform()->GetPosition();
	float vx = playerPosition.x - enemyPosition.x;
	float vy = playerPosition.y - enemyPosition.y;
	float vz = playerPosition.z - enemyPosition.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	if (dist < searchRange_)
	{
		float distXZ = sqrtf(vx * vx + vz * vz);
		//	�P�ʃx�N�g����
		vx /= distXZ;
		vz /= distXZ;

		//	�����x�N�g����
		float angleY = GetTransform()->GetRotationY();
		float frontX = sinf(angleY);
		float frontZ = cosf(angleY);
		//	2�̃x�N�g���̓��ϒl�őO�㔻��
		float dot = (frontX * vx) + (frontZ * vz);
		if (dot > 0.0f)
		{
			return true;
		}
	}
	return false;
}

//	�j��
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}

