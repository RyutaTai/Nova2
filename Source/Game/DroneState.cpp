#include "DroneState.h"

//	待機ステート
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

	void IdleState::DrawDebug()
	{
		if (ImGui::TreeNode("IdleState"))
		{

			ImGui::TreePop();
		}

	}

}

//	サーチステート
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

	void SearchState::DrawDebug()
	{
		if (ImGui::TreeNode("SearchState"))
		{

			ImGui::TreePop();
		}
	}

}

//	移動ステート
namespace DroneState
{
	void MoveState::Initialize()
	{

	}

	void MoveState::Update(const float& elapsedTime)
	{
		//owner_->GetTransform()->AddRotationY(10.0f * elapsedTime);	//	回転させてるだけ
	}

	void MoveState::Finalize()
	{

	}

	void MoveState::DrawDebug()
	{
		if (ImGui::TreeNode("MoveState"))
		{

			ImGui::TreePop();
		}
	}

}

//	攻撃ステート
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

	void AttackState::DrawDebug()
	{
		if (ImGui::TreeNode("AttackState"))
		{

			ImGui::TreePop();
		}
	}

}

//	回避ステート
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

	void AvoidanceState::DrawDebug()
	{
		if (ImGui::TreeNode("AvoidanceState"))
		{

			ImGui::TreePop();
		}
	}

}