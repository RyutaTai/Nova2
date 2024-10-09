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

private:	//	�X�v���C�g
	enum SPRITE_MODEL_CHECK
	{
		BACK,			//	�w�i�摜
		MAX,			//	�X�v���C�g�̏����
	};

	enum class MODEL_CHECK
	{
		SHOGI_BOARD,	//	������
		PLANTUNE,		//	�v�����`���[��
		CITY,			//	�V�e�B���f��
		DRONE,			//	�h���[��
		DRAGONKIN,		//	���l
		PLAYER,			//	�v���C���[
		CUBE,			//	�L���[�u
		MAX,
	};

	std::unique_ptr	<Sprite>					sprite_[static_cast<int>(SPRITE_MODEL_CHECK::MAX)];
	std::unique_ptr	<Character>					object_[static_cast<int>(MODEL_CHECK::MAX)];
	Microsoft::WRL::ComPtr	<ID3D11Buffer>		sceneConstantBuffer_;	
	Microsoft::WRL::ComPtr	<ID3D11PixelShader>	modelPixelShader_[static_cast<int>(MODEL_CHECK::MAX)];	//	���f�����Ƀs�N�Z���V�F�[�_�[���Z�b�g�ł���悤�ɂ���
	float										modelScale_ = 1.0f;

private:	//	�I�[�f�B�I
	enum class AUDIO_SE_MODEL_CHECK
	{
		CHOICE,		//	�I����
		DECISION,	//	���艹
		MAX,		//	SE�ő吔
	};
	enum class AUDIO_BGM_MODEL_CHECK
	{
		NORMAL,		//	�ʏ펞BGM
		MAX,		//	BGM�ő吔
	};
	std::unique_ptr<AudioSource> bgm_[static_cast<int>(AUDIO_BGM_MODEL_CHECK::MAX)];
	std::unique_ptr<AudioSource> se_[static_cast<int>(AUDIO_SE_MODEL_CHECK::MAX)];

	//	�Q�[���p�ϐ�
private:

};

