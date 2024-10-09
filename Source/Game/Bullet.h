#pragma once

#include "../Nova/Object/GameStaticObject.h"
#include "../Nova/Resources/Effect.h"
#include "../Game/BulletManager.h"
#include "../Nova/Audio/AudioSource3D.h"

//	�O���錾
//	���݃C���N���[�h���Ȃ��悤�ɑO���錾����
class BulletManager;

//	�e�ۃN���X
class Bullet :public GameStaticObject
{
public:
	Bullet(const std::string& fileName);
	virtual ~Bullet()override {}

	virtual void			Initialize();
	virtual void			Update(const float& elapsedTime);
	virtual void			Render()			= 0;
	virtual void			RnederCoverModel()	= 0;

	virtual void			CoverModelUpdate(const float& elapsedTime);									//	�J�o�[���f���X�V����
	virtual void			Destroy(const float& elapsedTime);											//	�j������
	
	void					SetDamaged(bool damaged)						{ damaged_ = damaged; }		//	�_���[�W�t���O�ݒ�
	void					SetOwnerPosition(const DirectX::XMFLOAT3& pos)	{ ownerPosition_ = pos; }	//	�e�ۏ��L�҂̈ʒu�ݒ�

	const DirectX::XMFLOAT3 GetOwnerPosition()								{ return ownerPosition_; }	//	�e�ۏ��L�҂̈ʒu�擾
	float					GetRadius()										{ return radius_; }			//	���a�擾

	void					DrawDebugPrimitive();														//	�f�o�b�O�v���~�e�B�u�`��
	virtual void			DrawDebug();																//	�f�o�b�O�`��

private:
	enum EFFECT_TYPE
	{
		SHORT = 0,
		LONG,
		MAX,
	};
	Effect* effect_[EFFECT_TYPE::MAX] = {};

	enum class AUDIO_SE_BULLET	//	3D�Ŗ炷SE
	{
		Explosion = 0,			//	������
		Max
	};

protected:
	DirectX::XMFLOAT3							velocity_ = {};					//	���x
	DirectX::XMFLOAT3							direction_ = {};				//	�e����ԕ���
	float										radius_;						//	�e�̔��a
	Microsoft::WRL::ComPtr	<ID3D11PixelShader>	coverPixelShader_;				//	�e�ۂ̃L���[�u�̃s�N�Z���V�F�[�_�[
	std::shared_ptr <GltfModelStaticBatching>	gltfStaticModelResource_;		//	Gltf���f��
	std::unique_ptr <GameStaticObject>			coverModel_ = {};				//	�e�̎���𕢂����f��
	bool										damaged_ = false;				//	�U�����󂯂����ǂ���

	DirectX::XMFLOAT3							ownerPosition_ = {};			//	�e�ۏ��L�҂̈ʒu

	std::unique_ptr<SoundEmitter> emitter_ = nullptr;							//	�G�~�b�^�[�������̈ʒu�Ŏ���
	std::unique_ptr<AudioSource3D> se_[static_cast<int>(AUDIO_SE_BULLET::Max)];	//	�e�ۂ�SE(3D�Ŗ炷)

};

