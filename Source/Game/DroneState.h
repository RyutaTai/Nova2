#pragma once

#include "../Nova/AI/State.h"
#include "Drone.h"

//	待機ステート
namespace DroneState
{
	class IdleState : public State<Drone>
	{
	public:
		IdleState(Drone* owner) : State(owner){}
		~IdleState(){}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	};
}

//	サーチステート
namespace DroneState
{
	class SearchState :public State<Drone>
	{
	public:
		SearchState(Drone* owner) : State(owner) {}
		~SearchState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	};
}

//	移動ステート
namespace DroneState
{
	class MoveState :public State<Drone>
	{
	public:
		MoveState(Drone* owner) : State(owner) {}
		~MoveState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	};
}

//	攻撃ステート
namespace DroneState
{
	class AttackState :public State<Drone>
	{
	public:
		AttackState(Drone* owner) : State(owner) {}
		~AttackState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

		void LaunchBullet();
		void Attack();

	};
}

//	回避ステート
namespace DroneState
{
	class AvoidanceState :public State<Drone>
	{
	public:
		AvoidanceState(Drone* owner) : State(owner) {}
		~AvoidanceState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	};
}
