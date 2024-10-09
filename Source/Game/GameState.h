#pragma once

#include "../Nova/AI/State.h"
#include "../Nova/Scenes/SceneGame.h"

//	�E�F�[�u1�i�G1�́j
namespace GameState
{
	class Wave1State : public State<SceneGame>
	{
	public:
		Wave1State(SceneGame* owner) : State(owner) {}
		~Wave1State() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;
	};
}

//	�E�F�[�u2�i�G2�́j
namespace GameState
{
	class Wave2State : public State<SceneGame>
	{
	public:
		Wave2State(SceneGame* owner) : State(owner) {}
		~Wave2State() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	};
}

//	�E�F�[�u3�i�G5�́j
namespace GameState
{
	class Wave3State : public State<SceneGame>
	{
	public:
		Wave3State(SceneGame* owner) : State(owner) {}
		~Wave3State() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	};
}

//	�{�X
namespace GameState
{
	class BossState : public State<SceneGame>
	{
	public:
		BossState(SceneGame* owner) : State(owner){}
		~BossState(){}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	};
}

//	�Q�[���N���A
namespace GameState
{
	class GameClearState : public State<SceneGame>
	{
	public:
		GameClearState(SceneGame* owner) : State(owner) {}
		~GameClearState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	private:
		float changeTitleTimer_ = 4.0f;

	};
}

//	���U���g
namespace GameState
{
	class GameOverState : public State<SceneGame>
	{
	public:
		GameOverState(SceneGame* owner) : State(owner) {}
		~GameOverState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	private:
		float changeTitleTimer_ = 4.0f;

	};
}
