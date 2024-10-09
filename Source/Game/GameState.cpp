#include "GameState.h"

#include "../Nova/Scenes/SceneGame.h"
#include "EnemyManager.h"

//	�E�F�[�u1�i�G1�́j
namespace GameState
{
	void Wave1State::Initialize()
	{
		EnemyManager::Instance().DroneSpawn(1);
		owner_->LoadWaveSprite(L"./Resources/Image/Wave1.png");
		owner_->SetWaveStartTimer(2.0f);
	}

	void Wave1State::Update(const float& elapsedTime)
	{
		//	�E�F�[�u�J�n�^�C�}�[�X�V
		float waveStartTimer = owner_->GetWaveStartTimer();
		waveStartTimer -= elapsedTime;
		if (waveStartTimer <= 0.0f)
		{
			waveStartTimer = 0.0f;
		}
		owner_->SetWaveStartTimer(waveStartTimer);

		//	�X�e�[�g�J��
		if (EnemyManager::Instance().GetEnemyCount() <= 0)
		{
			owner_->ChangeState(SceneGame::SceneGameState::Wave2);
		}

	}

	void Wave1State::Finalize()
	{

	}

}

//	�E�F�[�u2�i�G2�́j
namespace GameState
{
	void Wave2State::Initialize()
	{
		EnemyManager::Instance().DroneSpawn(3);
		owner_->LoadWaveSprite(L"./Resources/Image/Wave2.png");
		owner_->SetWaveStartTimer(2.0f);
	}

	void Wave2State::Update(const float& elapsedTime)
	{
		//	�E�F�[�u�J�n�^�C�}�[�X�V
		float waveStartTimer = owner_->GetWaveStartTimer();
		waveStartTimer -= elapsedTime;
		if (waveStartTimer <= 0.0f)
		{
			waveStartTimer = 0.0f;
		}
		owner_->SetWaveStartTimer(waveStartTimer);

		//	�X�e�[�g�J��
		if (EnemyManager::Instance().GetEnemyCount() <= 0)
		{
			owner_->ChangeState(SceneGame::SceneGameState::Wave3);
		}

	}

	void Wave2State::Finalize()
	{

	}
}

//	�E�F�[�u3�i�G5�́j
namespace GameState
{
	void Wave3State::Initialize()
	{
		EnemyManager::Instance().DroneSpawn(5);
		owner_->LoadWaveSprite(L"./Resources/Image/Wave3.png");
		owner_->SetWaveStartTimer(2.0f);
	}

	void Wave3State::Update(const float& elapsedTime)
	{
		//	�E�F�[�u�J�n�^�C�}�[�X�V
		float waveStartTimer = owner_->GetWaveStartTimer();
		waveStartTimer -= elapsedTime;
		if (waveStartTimer <= 0.0f)
		{
			waveStartTimer = 0.0f;
		}
		owner_->SetWaveStartTimer(waveStartTimer);

		//	�X�e�[�g�J��
		if (EnemyManager::Instance().GetEnemyCount() <= 0)
		{
			owner_->ChangeState(SceneGame::SceneGameState::Clear);
		}

	}

	void Wave3State::Finalize()
	{

	}
}

//	�Q�[���N���A
namespace GameState
{
	void GameClearState::Initialize()
	{
		owner_->SetGameClear(true);
		owner_->SetIsResult(true);
	}

	void GameClearState::Update(const float& elapsedTime)
	{
		changeTitleTimer_ -= elapsedTime;
		if (changeTitleTimer_ <= 0.0f)
		{
			owner_->ChangeToTitle(true);
			changeTitleTimer_ = 3.0f;
		}
	}

	void GameClearState::Finalize()
	{

	}
}

//	�Q�[���I�[�o�[
namespace GameState
{
	void GameOverState::Initialize()
	{
		owner_->SetGameOver(true);
		owner_->SetIsResult(true);
	}

	void GameOverState::Update(const float& elapsedTime)
	{
		changeTitleTimer_ -= elapsedTime;
		if (changeTitleTimer_ <= 0.0f)
		{
			owner_->ChangeToTitle(true);
			changeTitleTimer_ = 3.0f;
		}
	}

	void GameOverState::Finalize()
	{
		
	}
}

