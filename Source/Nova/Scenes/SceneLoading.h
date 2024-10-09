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
		BACK,				//	�w�i�摜
		LOADING,			//	���[�f�B���O�@�e�L�X�g
		TITLE_TEXT,
		MAX,				//	�X�v���C�g�̏����
	};
	std::unique_ptr	<Sprite> sprite_[static_cast<int>(SPRITE_LOADING::MAX)];

	//	���[�f�B���O�X���b�h
	static void LoadingThread(SceneLoading* scene);

private:
	Scene*						nextScene_	= nullptr;
	std::thread*				thread_		= nullptr;
	//std::unique_ptr	<Sprite>	sprite_		= nullptr;
	float angle_ = 0.0f;

	int animationNumber_		= 0;		//	���݂̃A�j���[�V������(�����ڂ̃A�j���[�V������)
	int animationMAX_			= 5;		//	�摜�̃A�j���[�V��������
	float animationFrame_		= 120.0f;	//	���̃R�}�ɍs���܂ł̃t���[����
	float animationTimer_		= 0;		//	���݂̃t���[����(�A�j���[�V�����^�C�}�[)
};

