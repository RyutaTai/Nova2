#include "TitleState.h"

#include "../Nova/Scenes/SceneManager.h"
#include "../Nova/Scenes/SceneTitle.h"
#include "../Nova/Scenes/SceneLoading.h"
#include "../Nova/Scenes/SceneGame.h"

//	Main
namespace TitleState
{
	void MainState::Initialize()
	{
		
	}

	void MainState::Update(const float& elapsedTime)
	{
		GamePad& gamePad = Input::Instance().GetGamePad();
		if (gamePad.GetButtonDown() & GamePad::BTN_A)   //  コントローラAキー
		{
			owner_->PlaySE(SceneTitle::AUDIO_SE_TITLE::DECISION);
			owner_->ChangeState(SceneTitle::SceneTitleState::Fade);

			//SceneManager::Instance().ChangeScene(new SceneGame);

		}
	}

	void MainState::Finalize()
	{

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
}

//	Fade
namespace TitleState
{
	void FadeState::Initialize()
	{
		fadeTimer_ = 0.0f;
	}

	void FadeState::Update(const float& elapsedTime)
	{
		fadeTimer_ += elapsedTime;
		if (FADE_TIME < fadeTimer_)	//	フェード完了したら
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
		}
	}

	void FadeState::Finalize()
	{

	}
}
