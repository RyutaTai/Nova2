#include "SceneTitle.h"

#include "../Graphics/Graphics.h"
#include "../Resources/Texture.h"
#include "../Scenes/SceneManager.h"
#include "../Scenes/SceneLoading.h"
#include "../Scenes/SceneGame.h"
#include "../../Game/UI/UIManager.h"
#include "../../Game/TitleState.h"
#include "../Audio/AudioManager.h"

//	初期化
void SceneTitle::Initialize()
{
	//	オーディオ初期化
#if UseOldAudioManager
	bgm_[static_cast<int>(AUDIO_BGM_TITLE::Title)] = AudioManager::Instance().LoadAudioSource("./Resources/Audio/BGM/Title.wav");
	bgm_[static_cast<int>(AUDIO_BGM_TITLE::Title)]->SetVolume(0.3f, false);

	se_[static_cast<int>(AUDIO_SE_TITLE::Decision)] = AudioManager::Instance().LoadAudioSource("./Resources/Audio/SE/Decision.wav");
	se_[static_cast<int>(AUDIO_SE_TITLE::Decision)]->SetVolume(0.2f, false);

#else
	AudioSource* titleBGM = AudioManager::Instance().LoadAudioSource("./Resources/Audio/BGM/Title.wav", Audio::AudioType::BGMNormal, "TitleScene");
	titleBGM->SetVolume(0.3f, false);
	titleBGM->SetAudioName("TitleBGM");
	AudioManager::Instance().Register(titleBGM);
	AudioManager::Instance().GetAudioResource("TitleBGM")->Play(true);

	AudioSource* decision = AudioManager::Instance().LoadAudioSource("./Resources/Audio/SE/Decision.wav", Audio::AudioType::SENormal, "TitleScene");
	decision->SetVolume(0.2f, false);
	decision->SetAudioName("Decision");
	AudioManager::Instance().Register(decision);
#endif

	//	スプライト初期化
	sprite_[static_cast<int>(SPRITE_TITLE::Back)] = std::make_unique<Sprite>(L"./Resources/Image/Back2.png");
	sprite_[static_cast<int>(SPRITE_TITLE::Groove)] = std::make_unique<Sprite>(L"./Resources/Image/Groove2.png");
	sprite_[static_cast<int>(SPRITE_TITLE::KeyText)] = std::make_unique<Sprite>(L"./Resources/Image/KeyText2.png");
	sprite_[static_cast<int>(SPRITE_TITLE::KeyText)]->GetTransform()->SetPosition(640, 855);
	//  UI
#if 0
	ui_ = new UI("./Resources/Image/KeyText2.png");
	ui_->GetTransform()->SetPosition({ 355,640 });
	ui_->SetName("KeyText");
	UIManager::Instance().Initialize();
#endif

	//  オーディオ初期化
	//audioInstance_.Initialize();
	//bgm_[static_cast<int>(AUDIO_BGM_TITLE::TITLE)] = std::make_unique<Audio>();
	//bgm_[static_cast<int>(AUDIO_BGM_TITLE::TITLE)]->SetVolume(0.3f);

	//se_[static_cast<int>(AUDIO_SE_TITLE::DECISION)] = std::make_unique<Audio>(audioInstance_.GetXAudio2(), L"./Resources/Audio/SE/GameStart_015.wav");
	//bgm_[0] = std::make_unique<Audio>(audioInstance_.GetXAudio2(), L"./Resources/Audio/BGM/009.wav");
	/*se_[0] = std::make_unique<Audio>(audioInstance_.GetXAudio2(), L"./Resources/Audio/SE/0footsteps-of-a-runner-on-gravel.wav");
	se_[1] = std::make_unique<Audio>(audioInstance_.GetXAudio2(), L"./Resources/Audio/SE/0footsteps-dry-leaves-g.wav");
	se_[2] = std::make_unique<Audio>(audioInstance_.GetXAudio2(), L"./Resources/Audio/SE/0explosion-8-bit.wav");*/

	//	ステート更新処理
	stateMachine_.reset(new StateMachine<State<SceneTitle>>());
	stateMachine_->RegisterState(new TitleState::MainState(this));		//	Main
	stateMachine_->RegisterState(new TitleState::SettingState(this));	//	Setting
	stateMachine_->RegisterState(new TitleState::FadeState(this));		//	Fade
	//	初期ステート設定
	stateMachine_->SetState(static_cast<int>(SceneTitleState::Main));	//	初期ステートセット

}

//	終了化
void SceneTitle::Finalize()
{
	//	スプライト終了化
	for (int i = 0; i < static_cast<int>(SPRITE_TITLE::Max); i++)
	{
		if (sprite_[i] != nullptr)
		{
			sprite_[i] = nullptr;
		}
	}

	//  UI終了化
	UIManager::Instance().Finalize();

	//	オーディオ終了化
	AudioManager::Instance().RemoveByScene("TitleScene");

}

//	更新処理
void SceneTitle::Update(const float& elapsedTime)
{
	const GamePadButton anyButton =
		GamePad::BTN_A		//Z
		| GamePad::BTN_B	//X
		| GamePad::BTN_X	//C
		| GamePad::BTN_Y;	//V

	/* ----- ステートマシン更新 ----- */
	stateMachine_->Update(elapsedTime);


	//se_[static_cast<int>(AUDIO_SE_TITLE::Decision)]->Update(elapsedTime);		//	プレイタイマー更新用

#if UseOldAudioManager
	bgm_[static_cast<int>(AUDIO_BGM_TITLE::Title)]->Play(true);
#else
	//AudioManager::Instance().GetAudioResource("Title.wav")->Play(true);
#endif
}

//  Shadow描画
void SceneTitle::ShadowRender()
{

}

//	描画処理
void SceneTitle::Render()
{
	//	タイトルスプライト描画
	Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_ON_ZW_ON);
	Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ALPHA);
	Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::CULL_NONE);

	//  背景
	sprite_[static_cast<int>(SPRITE_TITLE::Back)]->Render();

	//  タイトル文字
	static float grooveAlpha = 0.0f;
	grooveAlpha += 0.01f;
	Sprite* groove = sprite_[static_cast<int>(SPRITE_TITLE::Groove)].get();
	DirectX::XMFLOAT2 texSize = groove->GetTransform()->GetSize();
	groove->GetTransform()->SetColorA(grooveAlpha);

	DirectX::XMFLOAT4 color = { 1,1,1,1 };
	groove->GetTransform()->SetColor(color);
	color = groove->GetTransform()->GetColor();
	groove->Render();

	sprite_[static_cast<int>(SPRITE_TITLE::KeyText)]->Render();

	//  UI描画
#if 0
	UIManager::Instance().Render();
#endif
}

//	デバッグ描画
void SceneTitle::DrawDebug()
{
	if (ImGui::TreeNode("Sprite"))	//	タイトル画面
	{
		if (ImGui::TreeNode("Back"))	//	タイトル画面
		{
			sprite_[static_cast<int>(SPRITE_TITLE::Back)]->DrawDebug();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("KeyText"))
		{
			sprite_[static_cast<int>(SPRITE_TITLE::KeyText)]->DrawDebug();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("TitleText"))
		{
			sprite_[static_cast<int>(SPRITE_TITLE::Groove)]->DrawDebug();
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
		//se_[static_cast<int>(AUDIO_SE_TITLE::Decision)]->DrawDebug();
		ImGui::TreePop();
	}
}


