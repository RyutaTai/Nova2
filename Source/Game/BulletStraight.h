#pragma once

#include "Bullet.h"

class BulletManager;

//	���i�e��
class BulletStraight :public Bullet
{
public:
	BulletStraight(const std::string& fileName);
	~BulletStraight()override;

	//	�X�V����
	void Update(const float& elapsedTime)override;

	//	�`�揈��
	void Render()override;
	void RnederCoverModel()override {}

	//	����
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	float				speed_ = 300.0f;		//	���̑���
	float				lifeTimer_ = 3.0f;		//	���̐�������

};