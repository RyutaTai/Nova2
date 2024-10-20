#pragma once

#include "Scene.h"
#include "../Resources/Sprite.h"
#include "../Input/Input.h"
#include "../Audio/Audio.h"
#include "../Graphics/ShadowMap.h"
#include "../../Game/Stage.h"
#include "../../Game/Player.h"
#include "../../Game/Dragonkin.h"
#include "../../Game/Drone.h"
#include "../PostProcess/Bloom.h"
#include "../../Game/UI.h"

class SceneGame : public Scene
{
public:
	//	ステート
	enum class SceneGameState
	{
		Wave1,
		Wave2,
		Wave3,
		Clear,
		GameOver,
		Max,
	};

public:
	SceneGame(){}
	~SceneGame()override{}

	void Initialize()						override;
	void Finalize()							override;

	void Update(const float& elapsedTime)	override;
	void ShadowRender() 					override;
	void Render()							override;
	void DrawDebug()						override;

	void ChangeState(SceneGameState state) { GetStateMachine()->ChangeState(static_cast<int>(state)); }	//	ステート遷移

	void LoadWaveSprite(const wchar_t* fileName);
	StateMachine<State<SceneGame>>* GetStateMachine() { return stateMachine_.get(); }	//	ステートマシン取得
	void IsPose(bool isPose);
	void Reset();

	void SetChangeTitleTimer(int changeTitleTimer) { changeTitleTimer_ = changeTitleTimer; }
	void ChangeToTitle(bool changeTitle) { changeTitle_ = changeTitle; }
	void SetWaveStartTimer(float timer) { waveStartTimer_ = timer; }
	void SetGameOver(bool gameOver) { isGameOver_ = gameOver; }
	void SetGameClear(bool gameClear) { isGameClear_ = gameClear; }
	void SetIsResult(bool isResult) { isResult_ = isResult; }

	float	GetWaveStartTimer() { return waveStartTimer_; }
	float	GetChangeTitleTimer() { return changeTitleTimer_; }
	bool	GetIsResult() { return isResult_; }

	//	ゲーム用変数
private:
	/* ----- オブジェクト ----- */
	std::unique_ptr	<Stage>		stage_[2];	
	std::unique_ptr	<Player>	player_;
	std::unique_ptr	<Dragonkin>	dragonkin_;
	Drone*		drone_[5];
	std::unique_ptr<StateMachine<State<SceneGame>>>	stateMachine_ = nullptr;		//	ステートマシン

	/* ----- 描画関係 ----- */
	std::unique_ptr<GltfModel>					gltfModel_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceViews_[8];
	std::unique_ptr<Bloom>						bloomer_			= nullptr;		//	BLOOM
	std::unique_ptr<FrameBuffer>				framebuffers_[8];
	std::unique_ptr<FullScreenQuad>				bitBlockTransfer_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShaders_[8];
	DirectX::XMFLOAT4							lightDirection_		= { 0,-1,0,0 };
	float										nearZ_				= 50.0f;
	float										farZ_				= 400000.0f;

	/* ----- スプライト ----- */
	enum SPRITE_GAME
	{
		BACK,			//	背景画像
		WAVE,			//	ウェーブ
		RESULT,			//	リザルト
		HpFrame,		//	HPゲージ枠
		HpGauge,		//	HPゲージ
		HpGaugeBack,	//	HPゲージの減少量を描画する用
		Instructions,	//	操作方法
		Clear,			//	クリア
		GameOver,		//	ゲームオーバー
		MAX,			//	スプライトの上限数
	};

	enum class UI_GAME
	{
		HpGaugeBack,		//	HPゲージ(HPの減少量を見せるため)
		HpGauge,			//	メインのHPゲージ
		HpFrame,			//	HP枠
		Instructions,		//	操作方法
		Max,				//	UIの上限数
	};

	std::unique_ptr <Sprite>			  sprite_[static_cast<int>(SPRITE_GAME::MAX)];
	Microsoft::WRL::ComPtr <ID3D11Buffer> sceneConstantBuffer_;
	UI* ui_[static_cast<int>(UI_GAME::Max)];		//	UI


	/* ----- ゲーム内で使う変数 ----- */
	float	waveStartTimer_		= 0.0f;		//	ウェーブ開始のUIが表示されている間
	bool	isGameOver_			= false;
	bool	changeTitle_		= false;
	bool	isGameClear_		= false;
	float	changeTitleTimer_	= 3.0f;
	bool	isResult_			= false;	//	リザルト画面かどうか(クリア、ゲームオーバー)

	/* ----- オーディオ ----- */
	enum class AUDIO_SE_GAME
	{
		CHOICE,		//	選択音
		DECISION,	//	決定音
		MAX,		//	SE最大数
	};
	enum class AUDIO_BGM_GAME
	{
		NORMAL,		//	通常時BGM
		MAX,		//	BGM最大数
	};
	std::unique_ptr	<AudioSource> bgm_[static_cast<int>(AUDIO_BGM_GAME::MAX)];
	//std::unique_ptr	<Audio> se_[static_cast<int>(AUDIO_SE_GAME::MAX)];

	//	デバッグ用
private:
	bool drawUI_ = true;
};

