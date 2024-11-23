#include "SceneManager.h"

#include "../Audio/AudioManager.h"

//	デストラクタ
SceneManager::~SceneManager()
{
	if (currentScene_)	//	現在のシーン終了化
	{
		delete currentScene_;
		currentScene_ = nullptr;
	}
	if (nextScene_)	//	次のシーン終了化
	{
		delete nextScene_;
		nextScene_ = nullptr;
	}
}

//	更新処理
void SceneManager::Update(const float& elapsedTime)
{
	if (nextScene_ != nullptr)//	次のシーンが設定されていたらクリア
	{
		Clear();
		currentScene_ = nextScene_;
		nextScene_ = nullptr;

		if (!currentScene_->IsReady())
		{
			currentScene_->Initialize();
		}
	}

	if (currentScene_ != nullptr)
	{
		currentScene_->Update(elapsedTime);
	}
}

//	Shadow描画
void SceneManager::ShadowRender()
{
	if (currentScene_ != nullptr)
	{
		currentScene_->ShadowRender();
	}
}

//	描画処理
void SceneManager::Render()
{
	if (currentScene_ != nullptr)
	{
		currentScene_->Render();
	}
}

//	デバッグ描画
void SceneManager::DrawDebug()
{
#ifdef USE_IMGUI
	currentScene_->DrawDebug();
#endif// USE_IMGUI
}

//	シーンクリア
void SceneManager::Clear()
{
	if (currentScene_ != nullptr)
	{
		currentScene_->Finalize();
		delete currentScene_;
		currentScene_ = nullptr;
	}
}

//	シーン切り替え
void SceneManager::ChangeScene(Scene* scene)
{
	AudioManager::Instance().Clear();
	nextScene_ = scene;
}