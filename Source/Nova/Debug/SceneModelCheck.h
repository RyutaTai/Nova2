#pragma once

#include "../../Nova/Scenes/Scene.h"
#include "../Resources/Sprite.h"
#include "../../Game/Stage.h"
#include "../Audio/AudioSource.h"
#include "../../Game/Character.h"

class SceneModelCheck :public Scene
{
public:
	SceneModelCheck() {}
	~SceneModelCheck()override {}

	void Initialize()						override;
	void Finalize()							override;

	void Update(const float& elapsedTime)	override;
	void ShadowRender() 					override;
	void Render()							override;
	void DrawDebug()						override;

	void Reset();

private:	//	スプライト
	enum SPRITE_MODEL_CHECK
	{
		BACK,			//	背景画像
		MAX,			//	スプライトの上限数
	};

	enum class MODEL_CHECK
	{
		SHOGI_BOARD,	//	将棋盤
		PLANTUNE,		//	プランチューン
		CITY,			//	シティモデル
		DRONE,			//	ドローン
		DRAGONKIN,		//	竜人
		PLAYER,			//	プレイヤー
		CUBE,			//	キューブ
		MAX,
	};

	std::unique_ptr	<Sprite>					sprite_[static_cast<int>(SPRITE_MODEL_CHECK::MAX)];
	std::unique_ptr	<Character>					object_[static_cast<int>(MODEL_CHECK::MAX)];
	Microsoft::WRL::ComPtr	<ID3D11Buffer>		sceneConstantBuffer_;	
	Microsoft::WRL::ComPtr	<ID3D11PixelShader>	modelPixelShader_[static_cast<int>(MODEL_CHECK::MAX)];	//	モデル毎にピクセルシェーダーをセットできるようにする
	float										modelScale_ = 1.0f;

private:	//	オーディオ
	enum class AUDIO_SE_MODEL_CHECK
	{
		CHOICE,		//	選択音
		DECISION,	//	決定音
		MAX,		//	SE最大数
	};
	enum class AUDIO_BGM_MODEL_CHECK
	{
		NORMAL,		//	通常時BGM
		MAX,		//	BGM最大数
	};
	std::unique_ptr<AudioSource> bgm_[static_cast<int>(AUDIO_BGM_MODEL_CHECK::MAX)];
	std::unique_ptr<AudioSource> se_[static_cast<int>(AUDIO_SE_MODEL_CHECK::MAX)];

	//	ゲーム用変数
private:

};

