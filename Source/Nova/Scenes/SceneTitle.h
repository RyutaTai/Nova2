#pragma once

#include <memory>
#include <thread>

#include "Scene.h"
#include "../Resources/Sprite.h"
#include "../../Game/UI.h"
#include "../Audio/AudioSource.h"
#include "../AI/StateMachine.h"

class SceneTitle : public Scene
{
public:	
	//	ステート
	enum class SceneTitleState
	{
		Main,
		Setting,
		Fade,
		Max,
	};

	//	オーディオ
	enum class AUDIO_SE_TITLE
	{
		CHOICE,		//	選択音
		DECISION,	//	決定音
		Max,		//	SE最大数
	};
	enum class AUDIO_BGM_TITLE
	{
		TITLE,		//	タイトルBGM
		Max,		//	BGM最大数
	};

public:
	SceneTitle(){}
	~SceneTitle()override{}

	void Initialize()						override;
	void Finalize()							override;

	void Update(const float& elapsedTime)	override;
	void PlaySE(AUDIO_SE_TITLE seTitle);
	void PlayBGM(AUDIO_BGM_TITLE bgmTitle, bool loop);

	void ShadowRender() 					override;
	void Render()							override;

	void DrawDebug()						override;

	void ChangeState(SceneTitleState state) { stateMachine_->ChangeState(static_cast<int>(state)); }	//	ステート遷移
	StateMachine<State<SceneTitle>>* GetStateMachine() { return stateMachine_.get(); }					//	ステートマシン取得

private:
	std::unique_ptr<StateMachine<State<SceneTitle>>>	stateMachine_ = nullptr;		//	ステートマシン

private:	//	スプライト
	enum class SPRITE_TITLE
	{
		BACK,				//	背景画像
		GROOVE,				//	グルーブ テキストスプライト
		KEY_TEXT,			//	キーテキスト
		Max,				//	スプライトの上限数
	};
	std::unique_ptr	<Sprite> sprite_[static_cast<int>(SPRITE_TITLE::Max)];

	UI*	ui_;		//	UI

private:	
	//	オーディオ
	//std::unique_ptr	<Audio> bgm_[static_cast<int>(AUDIO_BGM_TITLE::Max)];
	//std::unique_ptr	<Audio> se_[static_cast<int>(AUDIO_SE_TITLE::Max)];

	std::shared_ptr<AudioSource> bgm_[static_cast<int>(AUDIO_BGM_TITLE::Max)];
	std::shared_ptr	<AudioSource> se_[static_cast<int>(AUDIO_SE_TITLE::Max)];


};

