#pragma once

#include <memory>
#include <thread>

#include "Scene.h"
#include "../Resources/Sprite.h"
#include "../../Game/UI.h"
#include "../Audio/AudioSource.h"

class SceneTitle : public Scene
{
private:	//	�^�C�g���̎��̑J�ڐ�
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

private:	//	�X�v���C�g
	enum class SPRITE_TITLE
	{
		BACK,				//	�w�i�摜
		GROOVE,				//	�O���[�u �e�L�X�g�X�v���C�g
		KEY_TEXT,			//	�L�[�e�L�X�g
		MAX,				//	�X�v���C�g�̏����
	};
	std::unique_ptr	<Sprite> sprite_[static_cast<int>(SPRITE_TITLE::MAX)];

	UI*	ui_;		//	UI

private:	//	�I�[�f�B�I
	enum class AUDIO_SE_TITLE
	{
		CHOICE,		//	�I����
		DECISION,	//	���艹
		MAX,		//	SE�ő吔
	};
	enum class AUDIO_BGM_TITLE
	{
		TITLE,		//	�^�C�g��BGM
		MAX,		//	BGM�ő吔
	};
	//std::unique_ptr	<Audio> bgm_[static_cast<int>(AUDIO_BGM_TITLE::MAX)];
	//std::unique_ptr	<Audio> se_[static_cast<int>(AUDIO_SE_TITLE::MAX)];

	std::unique_ptr<AudioSource> bgm_[static_cast<int>(AUDIO_BGM_TITLE::MAX)];
	std::unique_ptr	<AudioSource> se_[static_cast<int>(AUDIO_SE_TITLE::MAX)];


};

