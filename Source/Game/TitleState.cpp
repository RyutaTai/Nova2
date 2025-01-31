#include "TitleState.h"

#include "../Nova/Scenes/SceneManager.h"
#include "../Nova/Scenes/SceneTitle.h"
#include "../Nova/Scenes/SceneLoading.h"
#include "../Nova/Scenes/SceneGame.h"

//	FadeIn
namespace TitleState
{
	void FadeInState::Initialize()
	{
		logoAlpha_ = 0.0f;
	}

	void FadeInState::Update(const float& elapsedTime)
	{
		//	----- ステート経過時間更新 -----
		UpdateStateElapsedTime(elapsedTime);

		//  ----- タイトル文字のフェード -----
		logoAlpha_ += logoAlphaAdd_ * elapsedTime;

		//	ロゴのアルファ値が1.0f以上ならメインステートへ遷移
		if (logoAlpha_ >= 1.0f)
		{
			logoAlpha_ = 1.0f;
			owner_->ChangeState(SceneTitle::SceneTitleState::Main);
		}
		
		//	アルファ値を適応
		owner_->SetTitleLogoAlpha(logoAlpha_);
	}

	void FadeInState::Finalize()
	{

	}

	void FadeInState::DrawDebug()
	{
		if (ImGui::TreeNode("FadeInState"))
		{
			ImGui::DragFloat("LogoAlphaAdd", &logoAlphaAdd_, 0.01f);

			ImGui::TreePop();
		}
	}
}

//	Main
namespace TitleState
{
	void MainState::Initialize()
	{
		
	}

	void MainState::Update(const float& elapsedTime)
	{
		GamePad& gamePad = Input::Instance().GetGamePad();
		//	Aボタン(Zキー)が押されたらSEを鳴らし、Fadeステートへ遷移
		if (gamePad.GetButtonDown() & GamePad::BTN_A/*Zキー*/)
		{
			AudioManager::Instance().GetAudioResource("Decision")->Play(false);
			owner_->ChangeState(SceneTitle::SceneTitleState::FadeOut);

		}
	}

	void MainState::Finalize()
	{

	}

	void MainState::DrawDebug()
	{
		if (ImGui::TreeNode("MainState"))
		{

			ImGui::TreePop();
		}
	}

}

//	Setting
namespace TitleState
{
	void SettingState::Initialize()
	{
		
	}

	void SettingState::Update(const float& elapsedTime)
	{
		
	}

	void SettingState::Finalize()
	{

	}

	void SettingState::DrawDebug()
	{
		if (ImGui::TreeNode("SettingState"))
		{

			ImGui::TreePop();
		}
	}

}

//	FadeOut
namespace TitleState
{
	void FadeOutState::Initialize()
	{
		
	}

	void FadeOutState::Update(const float& elapsedTime)
	{
		//	----- ステート経過時間更新 -----
		UpdateStateElapsedTime(elapsedTime);

		//	フェードが終わったらローディングシーンへ遷移
		if (stateElapsedTime_ >= FadeOutTime_)
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
		}
	}

	void FadeOutState::Finalize()
	{

	}

	void FadeOutState::DrawDebug()
	{
		if (ImGui::TreeNode("FadeOutState"))
		{
			ImGui::DragFloat("FadeOutTime", &FadeOutTime_, 0.01f);

			ImGui::TreePop();
		}
	}

}
