#include "Bullet.h"

#include "../Nova/Debug/DebugRenderer.h"
#include "../Nova/Graphics/Graphics.h"
#include "../Nova/Collision/Collision.h"
#include "../Nova/Audio/Audio.h"

//	�R���X�g���N�^
Bullet::Bullet(const std::string& fileName)
	:GameStaticObject(fileName)
{
	//	�������Ƀ}�l�[�W���[�ɓo�^����
	BulletManager::Instance().Register(this);

	//	�e�ۃX�P�[���ݒ�(�`��p)
	float scale = 60.0f;
	GetTransform()->SetScaleFactor(scale);

	//	�e�۔��a(�����蔻��p)
	radius_ = 1.0f;

	//	SE�ǂݍ���
	emitter_ = std::make_unique<SoundEmitter>();
	emitter_->position = GetTransform()->GetPosition();
	emitter_->velocity = velocity_;
	emitter_->min_distance = 7.0f;
	emitter_->max_distance = 12.0f;
	emitter_->volume_ = 1.0f;
	//se_[static_cast<int>(AUDIO_SE_BULLET::Explosion)] = std::unique_ptr<AudioSource3D>(Audio::Instance().LoadAudioSource3D("./Resources/Audio/SE/GameStart_015.wav", emitter_.get()));

}

//	����������
void Bullet::Initialize()
{

}

//	�X�V����
void Bullet::Update(const float& elapsedTime)
{

}

//	�J�o�[���f���X�V����
void Bullet::CoverModelUpdate(const float& elpasedTime)
{
	//	�ʒu�X�V
	DirectX::XMFLOAT3 bulletPos = this->GetTransform()->GetPosition();
	coverModel_->GetTransform()->SetPosition(bulletPos);
}

//	�j��
void Bullet::Destroy(const float& elapsedTime)
{
	//	�������Đ�
	//se_[static_cast<int>(AUDIO_SE_BULLET::Explosion)]->Play(false);

	//	�}�l�[�W���[���玩�����폜����
	BulletManager::Instance().Remove(this);
}

//	�f�o�b�O�v���~�e�B�u�`��
void Bullet::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//	�Փ˔���p�̃f�o�b�O����`��
	DirectX::XMFLOAT3 position = this->GetTransform()->GetPosition();
	debugRenderer->DrawSphere(position, radius_, DirectX::XMFLOAT4(0, 0, 0, 1));
}

//	�f�o�b�O�`��
void Bullet::DrawDebug()
{
	float scale = GetTransform()->GetScaleFactor();

	if (ImGui::TreeNode(u8"Bullet �e��"))
	{
		GetTransform()->DrawDebug();
		ImGui::DragFloat("Radius", &radius_, 1.0f, -FLT_MAX, FLT_MAX);	//	���a
		ImGui::DragFloat("Scale", &scale, 0.1f, 1.0f, FLT_MAX);			//	�X�P�[��
		ImGui::TreePop();
	}

	GetTransform()->SetScaleFactor(scale);

}