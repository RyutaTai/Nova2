#pragma once

#include <memory>
#include <thread>

#include "Scene.h"
#include "../Resources/Sprite.h"
#include "../../Game/UI.h"
#include "../Audio/AudioSource.h"

class SceneTitle : public Scene
{
private:	//	タイトルの次の遷移先
	enum class TITLE_SELECT
	{
		Play,
		Tutorial,
	};

public:
	SceneTitle(){}
	~SceneTitle()override{}

	void Initialize()						override;
	void Finalize()							override;

	void Update(const float& elapsedTime)	override;

	void ShadowRender() 					override;
	void Render()							override;

	void DrawDebug()						override;

private:	//	スプライト
	enum class SPRITE_TITLE
	{
		BACK,				//	背景画像
		GROOVE,				//	グルーブ テキストスプライト
		KEY_TEXT,			//	キーテキスト
		MAX,				//	スプライトの上限数
	};
	std::unique_ptr	<Sprite> sprite_[static_cast<int>(SPRITE_TITLE::MAX)];

	UI*	ui_;		//	UI

private:	//	オーディオ
	enum class AUDIO_SE_TITLE
	{
		CHOICE,		//	選択音
		DECISION,	//	決定音
		MAX,		//	SE最大数
	};
	enum class AUDIO_BGM_TITLE
	{
		TITLE,		//	タイトルBGM
		MAX,		//	BGM最大数
	};
	//std::unique_ptr	<Audio> bgm_[static_cast<int>(AUDIO_BGM_TITLE::MAX)];
	//std::unique_ptr	<Audio> se_[static_cast<int>(AUDIO_SE_TITLE::MAX)];

	std::unique_ptr<AudioSource> bgm_[static_cast<int>(AUDIO_BGM_TITLE::MAX)];
	std::unique_ptr	<AudioSource> se_[static_cast<int>(AUDIO_SE_TITLE::MAX)];


};

