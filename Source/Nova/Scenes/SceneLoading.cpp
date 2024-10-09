#include "SceneLoading.h"

#include "SceneManager.h"
#include "../Graphics/Graphics.h"
#include "../Core/Framework.h"
#include "../Others/Transform.h"
#include "../Others/MathHelper.h"

//	初期化
void SceneLoading::Initialize()
{
	//	スプライト初期化
	sprite_[static_cast<int>(SPRITE_LOADING::LOADING)] = std::make_unique<Sprite>( L"./Resources/Image/Loading.png");
	sprite_[static_cast<int>(SPRITE_LOADING::BACK)] = std::make_unique<Sprite>(L"./Resources/Image/Back.png");
	sprite_[static_cast<int>(SPRITE_LOADING::TITLE_TEXT)] = std::make_unique<Sprite>( L"./Resources/Image/Groove.png");
	//sprite_ = std::make_unique<Sprite>( L"./Resources/Image/NowLoading.png");

	//	スレッド開始
	thread_ = new std::thread(LoadingThread, this);	//	LoadingThread関数にthisを渡す

}

//	終了化
void SceneLoading::Finalize()
{
	//	スレッド終了化
	if (thread_ != nullptr)
	{
		thread_->join();	//	処理が終わるまで待つ
		delete thread_;
		thread_ = nullptr;
	}

	//	スプライト終了化
	for (int i = 0; i < static_cast<int>(SPRITE_LOADING::MAX); i++)
	{
		if (sprite_[i] != nullptr)
		{
			sprite_[i] = nullptr;
		}
	}
}

//	更新処理
void SceneLoading::Update(const float& elapsedTime)
{
	//	次のシーンの準備が完了したらシーンを切り替える
	if (nextScene_->IsReady()) {
		SceneManager::Instance().ChangeScene(nextScene_);
		nextScene_ = nullptr;
	}

	//	描画位置設定
	DirectX::XMFLOAT2 pos = {590,430};
	sprite_[static_cast<int>(SPRITE_LOADING::LOADING)]->GetTransform()->SetPosition(pos);

	//	角度更新
	static float angle = 0.0f;
	angle += 90.0f * elapsedTime;
	sprite_[static_cast<int>(SPRITE_LOADING::LOADING)]->GetTransform()->SetAngle(angle);

}

void SceneLoading::ShadowRender()
{

}

//	描画処理
void SceneLoading::Render()
{
	//	Sprite
	{
#if 0
		//	ローディング画面を描画
		float spriteWidth = 1280.0f;
		float spriteHeight = 720.0f;
		sprite_->GetTransform()->SetSize({ spriteWidth,spriteHeight });
		//sprite_->GetTransform()->SetSize({ 5120,720 });//5120では無理?
		sprite_->GetTransform()->SetTexSize({ spriteWidth,spriteHeight });

		animationTimer_++;	//	elapsedTimeを関数で取得して足したほうがいい？ 速くなったり遅くなったりする
		//animationTimer += this->timeAddend + this->GetElapsedTime();
		if (animationTimer_ > animationFrame_)
		{
			float x = animationNumber_ * spriteWidth;
			sprite_->GetTransform()->SetTexPosX(x);					//	次のコマに移動
			animationTimer_ = 0;										//	アニメーションタイマーをリセット
			animationNumber_++;										//	アニメーション数を増やす
			if (animationNumber_ > animationMAX_)animationNumber_ = 0;	//	アニメーション数をリセット
		}
		sprite_->Render();
#endif

		sprite_[static_cast<int>(SPRITE_LOADING::BACK)]->Render();
		sprite_[static_cast<int>(SPRITE_LOADING::TITLE_TEXT)]->Render();
		sprite_[static_cast<int>(SPRITE_LOADING::LOADING)]->Render();
	}
}

//	ローディングスレッド
void SceneLoading::LoadingThread(SceneLoading* scene)
{
	//	COM関連の初期化でスレッド毎に呼ぶ必要がある
	CoInitialize(nullptr);

	//	次のシーンの初期化を行う
	scene->nextScene_->Initialize();

	//	スレッドが終わる前にCOM関連の終了化
	CoUninitialize();

	//	次のシーンの準備完了設定
	scene->nextScene_->SetReady();

}

//	デバッグ描画
void SceneLoading::DrawDebug()
{
	//	スプライト終了化
	for (int i = 0; i < static_cast<int>(SPRITE_LOADING::MAX); i++)
	{
		if (sprite_[i] != nullptr)
		{
			sprite_[i]->DrawDebug();
		}
	}
}