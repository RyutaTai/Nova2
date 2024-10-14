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
	//	�X�e�[�g
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

	void ChangeState(SceneGameState state) { stateMachine_->ChangeState(static_cast<int>(state)); }	//	�X�e�[�g�J��

	void LoadWaveSprite(const wchar_t* fileName);
	StateMachine<State<SceneGame>>* GetStateMachine() { return stateMachine_.get(); }	//	�X�e�[�g�}�V���擾
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

	//	�Q�[���p�ϐ�
private:
	/* ----- �I�u�W�F�N�g ----- */
	std::unique_ptr	<Stage>		stage_[2];	
	std::unique_ptr	<Player>	player_;
	std::unique_ptr	<Dragonkin>	dragonkin_;
	Drone*		drone_[5];
	std::unique_ptr<StateMachine<State<SceneGame>>>	stateMachine_ = nullptr;		//	�X�e�[�g�}�V��

	/* ----- �`��֌W ----- */
	std::unique_ptr<GltfModel>					gltfModel_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceViews_[8];
	std::unique_ptr<Bloom>						bloomer_			= nullptr;		//	BLOOM
	std::unique_ptr<FrameBuffer>				framebuffers_[8];
	std::unique_ptr<FullScreenQuad>				bitBlockTransfer_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShaders_[8];
	DirectX::XMFLOAT4							lightDirection_		= { 0,-1,0,0 };
	float										nearZ_				= 50.0f;
	float										farZ_				= 400000.0f;

	/* ----- �X�v���C�g ----- */
	enum SPRITE_GAME
	{
		BACK,			//	�w�i�摜
		WAVE,			//	�E�F�[�u
		RESULT,			//	���U���g
		HpFrame,		//	HP�Q�[�W�g
		HpGauge,		//	HP�Q�[�W
		HpGaugeBack,	//	HP�Q�[�W�̌����ʂ�`�悷��p
		Instructions,	//	������@
		Clear,			//	�N���A
		GameOver,		//	�Q�[���I�[�o�[
		MAX,			//	�X�v���C�g�̏����
	};

	enum class UI_GAME
	{
		HpGaugeBack,		//	HP�Q�[�W(HP�̌����ʂ������邽��)
		HpGauge,			//	���C����HP�Q�[�W
		HpFrame,			//	HP�g
		Instructions,		//	������@
		Max,				//	UI�̏����
	};

	std::unique_ptr <Sprite>			  sprite_[static_cast<int>(SPRITE_GAME::MAX)];
	Microsoft::WRL::ComPtr <ID3D11Buffer> sceneConstantBuffer_;
	UI* ui_[static_cast<int>(UI_GAME::Max)];		//	UI


	/* ----- �Q�[�����Ŏg���ϐ� ----- */
	float	waveStartTimer_		= 0.0f;		//	�E�F�[�u�J�n��UI���\������Ă����
	bool	isGameOver_			= false;
	bool	changeTitle_		= false;
	bool	isGameClear_		= false;
	float	changeTitleTimer_	= 3.0f;
	bool	isResult_			= false;	//	���U���g��ʂ��ǂ���(�N���A�A�Q�[���I�[�o�[)

	/* ----- �I�[�f�B�I ----- */
	enum class AUDIO_SE_GAME
	{
		CHOICE,		//	�I����
		DECISION,	//	���艹
		MAX,		//	SE�ő吔
	};
	enum class AUDIO_BGM_GAME
	{
		Normal,		//	�ʏ펞BGM
		Max,		//	BGM�ő吔
	};
	std::unique_ptr	<AudioSource> bgm_[static_cast<int>(AUDIO_BGM_GAME::Max)];
	//std::unique_ptr	<Audio> se_[static_cast<int>(AUDIO_SE_GAME::MAX)];

	//	�f�o�b�O�p
private:
	bool drawUI_ = true;
};

