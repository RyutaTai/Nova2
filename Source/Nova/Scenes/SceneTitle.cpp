#include "SceneTitle.h"

#include "../Graphics/Graphics.h"
#include "../Resources/Texture.h"
#include "../Scenes/SceneManager.h"
#include "../Scenes/SceneLoading.h"
#include "../Scenes/SceneGame.h"
#include "../../Game/UIManager.h"
#include "../Audio/Audio.h"
#include "../../Game/TitleState.h"

//	初期化
void SceneTitle::Initialize()
{
	//	Audioより後にSpriteを呼ぶ
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
	for (int i = 0; i < static_cast<int>(SPRITE_TITLE::MAX); i++)
	{
		if (sprite_[i] != nullptr)
		{
			sprite_[i] = nullptr;
		}
	}

	//  UI終了化
	UIManager::Instance().Finalize();

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

	//	Enterキーを押したらゲームシーンへ切り替え
	// 	GamePad& gamePad = Input::Instance().GetGamePad();
	//if (gamePad.GetButtonDown() & GamePad::BTN_START)
	//{
	//    SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
	//}

	//if (gamePad.GetButtonDown() & GamePad::BTN_A)   //  コントローラAキー
	//{
	//	se_[static_cast<int>(AUDIO_SE_TITLE::DECISION)]->Play(false);
	//	se_[static_cast<int>(AUDIO_SE_TITLE::DECISION)]->SetVolume(0.5f,false);
	//	
	//	//SceneManager::Instance().ChangeScene(new SceneGame);

	//}
	se_[static_cast<int>(AUDIO_SE_TITLE::DECISION)]->Update(elapsedTime);		//	プレイタイマー更新用

	// 終了処理
	//if( GetAsyncKeyState(VK_ESCAPE) & 0x8000 ) exit(0);

	//	BGM再生
	//bgm_[static_cast<int>(AUDIO_BGM_TITLE::TITLE)]->Play();

#if 1
	bgm_[static_cast<int>(AUDIO_BGM_TITLE::TITLE)]->Play(true);
#endif
}

//	SEを再生(ステートマシン側で使用)
void SceneTitle::PlaySE(AUDIO_SE_TITLE seTitle)
{
	se_[static_cast<int>(seTitle)]->Play(false);
}

//	BGMを再生(ステートマシン側で使用)
void SceneTitle::PlayBGM(AUDIO_BGM_TITLE bgmTitle, bool loop)
{
	bgm_[static_cast<int>(bgmTitle)]->Play(loop);
}

//  Shadow描画
void SceneTitle::ShadowRender()
{

}

//	描画処理
void SceneTitle::Render()
{
	//  タイトルスプライト描画
	Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_ON_ZW_ON);
	Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ALPHA);
	Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::CULL_NONE);

	//  背景
	sprite_[static_cast<int>(SPRITE_TITLE::BACK)]->Render();

	//  タイトル文字
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


