#include "BulletStraight.h"

//	�R���X�g���N�^
BulletStraight::BulletStraight(const std::string& fileName)
	:Bullet(fileName)
{
	//	�X�P�[���ݒ�
	GetTransform()->SetScaleFactor(30.0f);
}

//	�f�X�g���N�^
BulletStraight::~BulletStraight()
{
	
}

//	�X�V����
void BulletStraight::Update(const float& elapsedTime)
{
	//	��������
	lifeTimer_ -=elapsedTime;
	if (lifeTimer_ <= 0.0f)
	{
		//	�������폜
		Destroy(elapsedTime);
	}

	//	�ړ�
	float speed = this->speed_ * elapsedTime;
	DirectX::XMFLOAT3 position = GetTransform()->GetPosition();
	position.x += direction_.x * speed;
	position.y += direction_.y * speed;
	position.z += direction_.z * speed;
	GetTransform()->SetPosition(position);

}

//	�`�揈��
void BulletStraight::Render()
{
	GameStaticObject::Render();
}

//	����
void BulletStraight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	this->direction_ = direction;
	this->GetTransform()->SetPosition(position);
}