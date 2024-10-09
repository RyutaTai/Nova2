#include "SceneLoading.h"

#include "SceneManager.h"
#include "../Graphics/Graphics.h"
#include "../Core/Framework.h"
#include "../Others/Transform.h"
#include "../Others/MathHelper.h"

//	������
void SceneLoading::Initialize()
{
	//	�X�v���C�g������
	sprite_[static_cast<int>(SPRITE_LOADING::LOADING)] = std::make_unique<Sprite>( L"./Resources/Image/Loading.png");
	sprite_[static_cast<int>(SPRITE_LOADING::BACK)] = std::make_unique<Sprite>(L"./Resources/Image/Back.png");
	sprite_[static_cast<int>(SPRITE_LOADING::TITLE_TEXT)] = std::make_unique<Sprite>( L"./Resources/Image/Groove.png");
	//sprite_ = std::make_unique<Sprite>( L"./Resources/Image/NowLoading.png");

	//	�X���b�h�J�n
	thread_ = new std::thread(LoadingThread, this);	//	LoadingThread�֐���this��n��

}

//	�I����
void SceneLoading::Finalize()
{
	//	�X���b�h�I����
	if (thread_ != nullptr)
	{
		thread_->join();	//	�������I���܂ő҂�
		delete thread_;
		thread_ = nullptr;
	}

	//	�X�v���C�g�I����
	for (int i = 0; i < static_cast<int>(SPRITE_LOADING::MAX); i++)
	{
		if (sprite_[i] != nullptr)
		{
			sprite_[i] = nullptr;
		}
	}
}

//	�X�V����
void SceneLoading::Update(const float& elapsedTime)
{
	//	���̃V�[���̏���������������V�[����؂�ւ���
	if (nextScene_->IsReady()) {
		SceneManager::Instance().ChangeScene(nextScene_);
		nextScene_ = nullptr;
	}

	//	�`��ʒu�ݒ�
	DirectX::XMFLOAT2 pos = {590,430};
	sprite_[static_cast<int>(SPRITE_LOADING::LOADING)]->GetTransform()->SetPosition(pos);

	//	�p�x�X�V
	static float angle = 0.0f;
	angle += 90.0f * elapsedTime;
	sprite_[static_cast<int>(SPRITE_LOADING::LOADING)]->GetTransform()->SetAngle(angle);

}

void SceneLoading::ShadowRender()
{

}

//	�`�揈��
void SceneLoading::Render()
{
	//	Sprite
	{
#if 0
		//	���[�f�B���O��ʂ�`��
		float spriteWidth = 1280.0f;
		float spriteHeight = 720.0f;
		sprite_->GetTransform()->SetSize({ spriteWidth,spriteHeight });
		//sprite_->GetTransform()->SetSize({ 5120,720 });//5120�ł͖���?
		sprite_->GetTransform()->SetTexSize({ spriteWidth,spriteHeight });

		animationTimer_++;	//	elapsedTime���֐��Ŏ擾���đ������ق��������H �����Ȃ�����x���Ȃ����肷��
		//animationTimer += this->timeAddend + this->GetElapsedTime();
		if (animationTimer_ > animationFrame_)
		{
			float x = animationNumber_ * spriteWidth;
			sprite_->GetTransform()->SetTexPosX(x);					//	���̃R�}�Ɉړ�
			animationTimer_ = 0;										//	�A�j���[�V�����^�C�}�[�����Z�b�g
			animationNumber_++;										//	�A�j���[�V�������𑝂₷
			if (animationNumber_ > animationMAX_)animationNumber_ = 0;	//	�A�j���[�V�����������Z�b�g
		}
		sprite_->Render();
#endif

		sprite_[static_cast<int>(SPRITE_LOADING::BACK)]->Render();
		sprite_[static_cast<int>(SPRITE_LOADING::TITLE_TEXT)]->Render();
		sprite_[static_cast<int>(SPRITE_LOADING::LOADING)]->Render();
	}
}

//	���[�f�B���O�X���b�h
void SceneLoading::LoadingThread(SceneLoading* scene)
{
	//	COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
	CoInitialize(nullptr);

	//	���̃V�[���̏��������s��
	scene->nextScene_->Initialize();

	//	�X���b�h���I���O��COM�֘A�̏I����
	CoUninitialize();

	//	���̃V�[���̏��������ݒ�
	scene->nextScene_->SetReady();

}

//	�f�o�b�O�`��
void SceneLoading::DrawDebug()
{
	//	�X�v���C�g�I����
	for (int i = 0; i < static_cast<int>(SPRITE_LOADING::MAX); i++)
	{
		if (sprite_[i] != nullptr)
		{
			sprite_[i]->DrawDebug();
		}
	}
}