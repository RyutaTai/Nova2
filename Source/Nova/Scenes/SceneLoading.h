#pragma once

#include <thread>

#include "Scene.h"
#include "../Resources/Sprite.h"

class SceneLoading :public Scene
{
public:
	SceneLoading(Scene* nextScene) :nextScene_(nextScene) {}
	~SceneLoading()override {}

	void Initialize()						override;
	void Finalize()							override;

	void Update(const float& elapsedTime)	override;

	void ShadowRender() 					override;
	void Render()							override;

	void DrawDebug()						override;

private:
	enum class SPRITE_LOADING
	{
		BACK,				//	背景画像
		LOADING,			//	ローディング　テキスト
		TITLE_TEXT,
		MAX,				//	スプライトの上限数
	};
	std::unique_ptr	<Sprite> sprite_[static_cast<int>(SPRITE_LOADING::MAX)];

	//	ローディングスレッド
	static void LoadingThread(SceneLoading* scene);

private:
	Scene*						nextScene_	= nullptr;
	std::thread*				thread_		= nullptr;
	//std::unique_ptr	<Sprite>	sprite_		= nullptr;
	float angle_ = 0.0f;

	int animationNumber_		= 0;		//	現在のアニメーション数(何枚目のアニメーションか)
	int animationMAX_			= 5;		//	画像のアニメーション枚数
	float animationFrame_		= 120.0f;	//	次のコマに行くまでのフレーム数
	float animationTimer_		= 0;		//	現在のフレーム数(アニメーションタイマー)
};

