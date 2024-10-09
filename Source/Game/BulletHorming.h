#pragma once

#include <memory>

#include "Bullet.h"
#include "../Nova/Resources/Effect.h"

class BulletHorming :public Bullet
{
public:
	BulletHorming(const std::string& fileName);
	~BulletHorming()override;

	//	�X�V����
	void Update(const float& elapsedTime)override;

	//	�`�揈��
	void Render()override;
	void RnederCoverModel()override;

	void Move(const float& elpasedTime);													//	�ړ�����
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);		//	����
	void Destroy(const float& elapsedTime)override;											//	�j������
	void SetTarget(const DirectX::XMFLOAT3& target) { target_ = target; }					//	�^�[�Q�b�g�ݒ�

	void DrawDebug();	//	�f�o�b�O�`��

private:
	enum EFFECT
	{
		FIRE = 0,		//	�e�̎���̃G�t�F�N�g
		EXPLOSION,		//	����
		MAX,			//	�ő吔
	};

private:
	float							speed_ = 200.0f;						//	�e�̑���
	//float							speed_		= 1.0f;						//	�e�̑���
	float							lifeTimer_ = 3.0f;						//	�e�̐�������
	DirectX::XMFLOAT3				target_ = { 0,0,0 };					//	�^�[�Q�b�g�ʒu
	std::shared_ptr <Effect>		effectResource_[EFFECT::MAX];			//	�G�t�F�N�g		
	float							effectScale_[EFFECT::MAX] = { 1.0f };	//	�G�t�F�N�g�X�P�[��

};

