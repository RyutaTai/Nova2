#include "GameState.h"

#include "../Nova/Scenes/SceneGame.h"
#include "EnemyManager.h"

//	ウェーブ1（敵1体）
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
		//	ウェーブ開始タイマー更新
		float waveStartTimer = owner_->GetWaveStartTimer();
		waveStartTimer -= elapsedTime;
		if (waveStartTimer <= 0.0f)
		{
			waveStartTimer = 0.0f;
		}
		owner_->SetWaveStartTimer(waveStartTimer);

		//	ステート遷移
		if (EnemyManager::Instance().GetEnemyCount() <= 0)
		{
			owner_->ChangeState(SceneGame::SceneGameState::Wave2);
		}

	}

	void Wave1State::Finalize()
	{

	}

}

//	ウェーブ2（敵2体）
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
		//	ウェーブ開始タイマー更新
		float waveStartTimer = owner_->GetWaveStartTimer();
		waveStartTimer -= elapsedTime;
		if (waveStartTimer <= 0.0f)
		{
			waveStartTimer = 0.0f;
		}
		owner_->SetWaveStartTimer(waveStartTimer);

		//	ステート遷移
		if (EnemyManager::Instance().GetEnemyCount() <= 0)
		{
			owner_->ChangeState(SceneGame::SceneGameState::Wave3);
		}

	}

	void Wave2State::Finalize()
	{

	}
}

//	ウェーブ3（敵5体）
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
		//	ウェーブ開始タイマー更新
		float waveStartTimer = owner_->GetWaveStartTimer();
		waveStartTimer -= elapsedTime;
		if (waveStartTimer <= 0.0f)
		{
			waveStartTimer = 0.0f;
		}
		owner_->SetWaveStartTimer(waveStartTimer);

		//	ステート遷移
		if (EnemyManager::Instance().GetEnemyCount() <= 0)
		{
			owner_->ChangeState(SceneGame::SceneGameState::Clear);
		}

	}

	void Wave3State::Finalize()
	{

	}
}

//	ゲームクリア
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

//	ゲームオーバー
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

