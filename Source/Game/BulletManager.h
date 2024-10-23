#pragma once

#include <vector>
#include <set>

#include "../Game/Bullet.h"

class Bullet;

class BulletManager
{
public:
	BulletManager();
	~BulletManager();

	static BulletManager& Instance()
	{
		static BulletManager instance;
		return instance;
	}

	//	����������
	void Initialize();

	//	�X�V����
	void Update(const float& elapsedTime);

	//	�J�o�[���f���X�P�[���X�V����
	void CoverModelUpdate(const float& elapsedTime);

	//	�`�揈��
	void Render();

	//	�e�ۓo�^
	void Register(Bullet* bullet);

	//	�e�ۑS�폜
	void Clear();

	//	�J�o�[���f���X�P�[���Z�b�g
	void SetCoverScale(const float& coverScale) { coverScale_ = coverScale; }

	//	�J�o�[���f���X�P�[���擾
	const float& GetCoverScale() const { return coverScale_; }

	//	���G��ԃZ�b�g
	void SetInvincible(bool isInvincible);

	//	�e�ې��擾
	int GetBulletCount()const { return static_cast<int>(bullets_.size()); }

	//	�e�ێ擾
	Bullet* GetBullet(const int& index) { return bullets_.at(index); }

	//	�e�ۍ폜
	void Remove(Bullet* projectile);

	//	�f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	//	�f�o�b�O�`��
	void DrawDebug();

private:
	std::vector	<Bullet*>	bullets_;
	std::set	<Bullet*>	removes_;
#if 0
	float					coverScale_		= 30.0f;	//	�J�o�[���f���̃X�P�[��
	float					scaleAdd_		= 10.0f;	//	�ǂ̂��炢scale�����Z���邩
	const float				COVER_SCALE_MAX = 50.0f;	//	�X�P�[�����
	const float				COVER_SCALE_MIN = 20.0f;	//	�X�P�[������	����������ƒe�ɖ��܂��Č����Ȃ�
#else
	//float					coverScale_ = 10;		//	�J�o�[���f���̃X�P�[��
	//float					scaleAdd_ = 5;			//	�ǂ̂��炢scale�����Z���邩
	//const float				COVER_SCALE_MAX = 12;	//	�X�P�[�����
	//const float				COVER_SCALE_MIN = 7;	//	�X�P�[������	����������ƒe�ɖ��܂��Č����Ȃ�

	float					coverScale_ = 0.1f;		//	�J�o�[���f���̃X�P�[��
	float					scaleAdd_ = 1.0f;		//	�ǂ̂��炢scale�����Z���邩
	const float				COVER_SCALE_MAX = 1.2f;	//	�X�P�[�����
	const float				COVER_SCALE_MIN = 0.7f;	//	�X�P�[������	����������ƒe�ɖ��܂��Č����Ȃ�

	float					coverScaleTimer_ = 0.0f;

#endif

private:	//	�f�o�b�O�p�̕ϐ�
	bool isInvincible_ = false;			//	���G���
	bool isBulletDestroy_ = false;		//	�e�۔j��
	bool coverModelDraw_ = true;		//	�J�o�[���f���`��t���O
	bool isCoverModelUpdate_ = true;	//	�J�o�[���f���X�V����

};