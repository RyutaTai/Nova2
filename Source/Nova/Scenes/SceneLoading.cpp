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
	sprite_[static_cast<int>(SPRITE_LOADING::Loading)] = std::make_unique<Sprite>( L"./Resources/Image/Loading.png");
	sprite_[static_cast<int>(SPRITE_LOADING::Loading)]->GetTransform()->SetPosition(900, 855);
	sprite_[static_cast<int>(SPRITE_LOADING::Loading)]->GetTransform()->AddPosition(50, 50);
	sprite_[static_cast<int>(SPRITE_LOADING::Loading)]->GetTransform()->SetPivot(0.5f, 0.5f);
	sprite_[static_cast<int>(SPRITE_LOADING::Back)] = std::make_unique<Sprite>(L"./Resources/Image/Back2.png");
	sprite_[static_cast<int>(SPRITE_LOADING::TitleText)] = std::make_unique<Sprite>( L"./Resources/Image/Groove2.png");
	//sprite_ = std::make_unique<Sprite>( L"./Resources/Image/NowLoading.png");

	//	スレッド開始
	thread_ = new std::thread(LoadingThread, this);	//	LoadingThread関数にthisを渡す

	//	ロード画像の角度初期化
	loadSpriteangle_ = 0.0f;

	//	オーディオ初期化
	AudioSource* loadBGM = AudioManager::Instance().LoadAudioSource("./Resources/Audio/BGM/Load.wav", Audio::AudioType::BGMNormal, "LoadingScene");
	loadBGM->SetVolume(0.2f, false);
	loadBGM->SetAudioName("LoadBGM");
	AudioManager::Instance().Register(loadBGM);
	AudioManager::Instance().GetAudioResource("LoadBGM")->Play(true);

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
	for (int i = 0; i < static_cast<int>(SPRITE_LOADING::Max); i++)
	{
		if (sprite_[i] != nullptr)
		{
			sprite_[i] = nullptr;
		}
	}

	//	オーディオ終了化
	AudioManager::Instance().RemoveByScene("LoadingScene");

}

//	更新処理
void SceneLoading::Update(const float& elapsedTime)
{
	//	次のシーンの準備が完了したらシーンを切り替える
	if (nextScene_->IsReady()) {
		SceneManager::Instance().ChangeScene(nextScene_);
		nextScene_ = nullptr;
	}

	//	角度更新
	loadSpriteangle_ += 90.0f * elapsedTime;
	if (loadSpriteangle_ > 360.0f)
	{
		loadSpriteangle_ = 0.0f;
	}
	sprite_[static_cast<int>(SPRITE_LOADING::Loading)]->GetTransform()->SetAngle(loadSpriteangle_);

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

		sprite_[static_cast<int>(SPRITE_LOADING::Back)]->Render();
		sprite_[static_cast<int>(SPRITE_LOADING::TitleText)]->Render();
		sprite_[static_cast<int>(SPRITE_LOADING::Loading)]->Render();
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
	for (int i = 0; i < static_cast<int>(SPRITE_LOADING::Max); i++)
	{
		if (sprite_[i] != nullptr)
		{
			sprite_[i]->DrawDebug();
		}
	}
}