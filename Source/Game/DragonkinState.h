#pragma once

#include "../Nova/AI/State.h"
#include "Dragonkin.h"

//	�ҋ@�X�e�[�g
namespace DragonkinState
{
	class IdleState :public State<Dragonkin>
	{
	public:
		IdleState(Dragonkin* owner) : State(owner) {}
		~IdleState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	};
}

//	�ړ��X�e�[�g
namespace DragonkinState
{
	class MoveState :public State<Dragonkin>
	{
	public:
		MoveState(Dragonkin* owner) : State(owner) {}
		~MoveState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	};
}

//	�U���X�e�[�g
namespace DragonkinState
{
	class AttackState :public State<Dragonkin>
	{
	public:
		AttackState(Dragonkin* owner) : State(owner) {}
		~AttackState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

		void Attack();

	};
}

//	����X�e�[�g
namespace DragonkinState
{
	class AvoidanceState :public State<Dragonkin>
	{
	public:
		AvoidanceState(Dragonkin* owner) : State(owner) {}
		~AvoidanceState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	};
}
