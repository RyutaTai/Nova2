#include "DroneState.h"

//	�ҋ@�X�e�[�g
namespace DroneState
{
	void IdleState::Initialize()
	{

	}

	void IdleState::Update(const float& elpasedTime)
	{
		owner_->ChangeState(Drone::StateType::Move);
	}

	void IdleState::Finalize()
	{

	}

}

//	�T�[�`�X�e�[�g
namespace DroneState
{
	void SearchState::Initialize()
	{

	}

	void SearchState::Update(const float& elapsedTime)
	{
		if (owner_->SearchPlayer())
		{
			owner_->ChangeState(Drone::StateType::Attack);
		}
	}

	void SearchState::Finalize()
	{

	}
}

//	�ړ��X�e�[�g
namespace DroneState
{
	void MoveState::Initialize()
	{

	}

	void MoveState::Update(const float& elapsedTime)
	{
		//owner_->GetTransform()->AddRotationY(10.0f * elapsedTime);	//	��]�����Ă邾��
	}

	void MoveState::Finalize()
	{

	}
}

//	�U���X�e�[�g
namespace DroneState
{
	void AttackState::Initialize()
	{

	}

	void AttackState::Update(const float& elapsedTime)
	{

	}

	void AttackState::Attack()
	{

	}

	void AttackState::LaunchBullet()
	{

	}

	void AttackState::Finalize()
	{

	}
}

//	����X�e�[�g
namespace DroneState
{
	void AvoidanceState::Initialize()
	{

	}

	void AvoidanceState::Update(const float& elapsedTime)
	{

	}

	void AvoidanceState::Finalize()
	{

	}
}