#include "SceneTitle.h"

#include "../Graphics/Graphics.h"
#include "../Resources/Texture.h"
#include "../Scenes/SceneManager.h"
#include "../Scenes/SceneLoading.h"
#include "../Scenes/SceneGame.h"
#include "../../Game/UIManager.h"
#include "../Audio/Audio.h"
#include "../../Game/TitleState.h"

//	������
void SceneTitle::Initialize()
{
	//	Audio�����Sprite���Ă�
#if 1
	bgm_[static_cast<int>(AUDIO_BGM_TITLE::TITLE)] = std::unique_ptr<AudioSource>(Audio::Instance().LoadAudioSource("./Resources/Audio/BGM/Title.wav"));
	bgm_[static_cast<int>(AUDIO_BGM_TITLE::TITLE)]->SetVolume(0.3f, false);

	se_[static_cast<int>(AUDIO_SE_TITLE::DECISION)] = std::unique_ptr<AudioSource>(Audio::Instance().LoadAudioSource("./Resources/Audio/SE/GameStart_015.wav"));
	se_[static_cast<int>(AUDIO_SE_TITLE::DECISION)]->SetVolume(0.5f, false);
#endif

	sprite_[static_cast<int>(SPRITE_TITLE::BACK)] = std::make_unique<Sprite>(L"./Resources/Image/Back.png");
	sprite_[static_cast<int>(SPRITE_TITLE::GROOVE)] = std::make_unique<Sprite>(L"./Resources/Image/Groove.png");
	sprite_[static_cast<int>(SPRITE_TITLE::KEY_TEXT)] = std::make_unique<Sprite>(L"./Resources/Image/KeyText2.png");

	//  UI
#if 0
	ui_ = new UI("./Resources/Image/KeyText2.png");
	ui_->GetTransform()->SetPosition({ 355,640 });
	ui_->SetName("KeyText");
	UIManager::Instance().Initialize();
#endif

	//  �I�[�f�B�I������
	//audioInstance_.Initialize();
	//bgm_[static_cast<int>(AUDIO_BGM_TITLE::TITLE)] = std::make_unique<Audio>();
	//bgm_[static_cast<int>(AUDIO_BGM_TITLE::TITLE)]->SetVolume(0.3f);

	//se_[static_cast<int>(AUDIO_SE_TITLE::DECISION)] = std::make_unique<Audio>(audioInstance_.GetXAudio2(), L"./Resources/Audio/SE/GameStart_015.wav");
	//bgm_[0] = std::make_unique<Audio>(audioInstance_.GetXAudio2(), L"./Resources/Audio/BGM/009.wav");
	/*se_[0] = std::make_unique<Audio>(audioInstance_.GetXAudio2(), L"./Resources/Audio/SE/0footsteps-of-a-runner-on-gravel.wav");
	se_[1] = std::make_unique<Audio>(audioInstance_.GetXAudio2(), L"./Resources/Audio/SE/0footsteps-dry-leaves-g.wav");
	se_[2] = std::make_unique<Audio>(audioInstance_.GetXAudio2(), L"./Resources/Audio/SE/0explosion-8-bit.wav");*/

	//	�X�e�[�g�X�V����
	stateMachine_.reset(new StateMachine<State<SceneTitle>>());
	stateMachine_->RegisterState(new TitleState::MainState(this));		//	Main
	stateMachine_->RegisterState(new TitleState::SettingState(this));	//	Setting
	stateMachine_->RegisterState(new TitleState::FadeState(this));		//	Fade
	//	�����X�e�[�g�ݒ�
	stateMachine_->SetState(static_cast<int>(SceneTitleState::Main));	//	�����X�e�[�g�Z�b�g

}

//	�I����
void SceneTitle::Finalize()
{
	for (int i = 0; i < static_cast<int>(SPRITE_TITLE::MAX); i++)
	{
		if (sprite_[i] != nullptr)
		{
			sprite_[i] = nullptr;
		}
	}

	//  UI�I����
	UIManager::Instance().Finalize();

}

//	�X�V����
void SceneTitle::Update(const float& elapsedTime)
{
	const GamePadButton anyButton =
		GamePad::BTN_A		//Z
		| GamePad::BTN_B	//X
		| GamePad::BTN_X	//C
		| GamePad::BTN_Y;	//V

	/* ----- �X�e�[�g�}�V���X�V ----- */
	stateMachine_->Update(elapsedTime);

	//	Enter�L�[����������Q�[���V�[���֐؂�ւ�
	// 	GamePad& gamePad = Input::Instance().GetGamePad();
	//if (gamePad.GetButtonDown() & GamePad::BTN_START)
	//{
	//    SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
	//}

	//if (gamePad.GetButtonDown() & GamePad::BTN_A)   //  �R���g���[��A�L�[
	//{
	//	se_[static_cast<int>(AUDIO_SE_TITLE::DECISION)]->Play(false);
	//	se_[static_cast<int>(AUDIO_SE_TITLE::DECISION)]->SetVolume(0.5f,false);
	//	
	//	//SceneManager::Instance().ChangeScene(new SceneGame);

	//}
	se_[static_cast<int>(AUDIO_SE_TITLE::DECISION)]->Update(elapsedTime);		//	�v���C�^�C�}�[�X�V�p

	// �I������
	//if( GetAsyncKeyState(VK_ESCAPE) & 0x8000 ) exit(0);

	//	BGM�Đ�
	//bgm_[static_cast<int>(AUDIO_BGM_TITLE::TITLE)]->Play();

#if 1
	bgm_[static_cast<int>(AUDIO_BGM_TITLE::TITLE)]->Play(true);
#endif
}

//	SE���Đ�(�X�e�[�g�}�V�����Ŏg�p)
void SceneTitle::PlaySE(AUDIO_SE_TITLE seTitle)
{
	se_[static_cast<int>(seTitle)]->Play(false);
}

//	BGM���Đ�(�X�e�[�g�}�V�����Ŏg�p)
void SceneTitle::PlayBGM(AUDIO_BGM_TITLE bgmTitle, bool loop)
{
	bgm_[static_cast<int>(bgmTitle)]->Play(loop);
}

//  Shadow�`��
void SceneTitle::ShadowRender()
{

}

//	�`�揈��
void SceneTitle::Render()
{
	//  �^�C�g���X�v���C�g�`��
	Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_ON_ZW_ON);
	Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ALPHA);
	Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::CULL_NONE);

	//  �w�i
	sprite_[static_cast<int>(SPRITE_TITLE::BACK)]->Render();

	//  �^�C�g������
	static float grooveAlpha = 0.0f;
	grooveAlpha += 0.01f;
	Sprite* groove = sprite_[static_cast<int>(SPRITE_TITLE::GROOVE)].get();
	DirectX::XMFLOAT2 texSize = groove->GetTransform()->GetSize();
	groove->GetTransform()->SetColorA(grooveAlpha);

	DirectX::XMFLOAT4 color = { 1,0,0,1 };
	groove->GetTransform()->SetColor(color);
	color = groove->GetTransform()->GetColor();
	groove->Render();

	sprite_[static_cast<int>(SPRITE_TITLE::KEY_TEXT)]->GetTransform()->SetPosition(355, 540);
	sprite_[static_cast<int>(SPRITE_TITLE::KEY_TEXT)]->Render();

	//  UI�`��
#if 0
	UIManager::Instance().Render();
#endif
}

//	�f�o�b�O�`��
void SceneTitle::DrawDebug()
{
	if (ImGui::TreeNode("Sprite"))	//	�^�C�g�����
	{
		if (ImGui::TreeNode("Back"))	//	�^�C�g�����
		{
			sprite_[static_cast<int>(SPRITE_TITLE::BACK)]->DrawDebug();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("UI"))
		{
			UIManager::Instance().DrawDebug();
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Audio"))
	{
		se_[static_cast<int>(AUDIO_SE_TITLE::DECISION)]->DrawDebug();
		ImGui::TreePop();
	}
}


