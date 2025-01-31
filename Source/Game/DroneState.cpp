#include "DroneState.h"

//	待機ステート
namespace DroneState
{
	void IdleState::Initialize()
	{
		
	}

	void IdleState::Update(const float& elapsedTime)
	{
		//	----- 経過時間更新 -----
		UpdateStateElapsedTime(elapsedTime);

		//	待機時間が経過したら索敵ステートへ遷移
		if (stateElapsedTime_ > IdleTime_)
		{
			owner_->ChangeState(Drone::StateType::Search);
			return;
		}
	}

	void IdleState::Finalize()
	{

	}

	void IdleState::DrawDebug()
	{
		if (ImGui::TreeNode("IdleState"))
		{
			ImGui::DragFloat("IdleTime", &IdleTime_, 0.1f);
			ImGui::DragFloat("ElapsedTime", &stateElapsedTime_, 0.1f);

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
		//	----- 経過時間更新 -----
		UpdateStateElapsedTime(elapsedTime);

		//	プレイヤーを見つけたら移動ステートへ遷移
		if (owner_->SearchPlayer())
		{
			owner_->ChangeState(Drone::StateType::Pursuit);
			return;
		}

	}

	void SearchState::Finalize()
	{

	}

	void SearchState::DrawDebug()
	{
		if (ImGui::TreeNode("SearchState"))
		{
			ImGui::DragFloat("ElapsedTIme", &stateElapsedTime_, 0.1f);

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
		//	----- 経過時間更新 -----
		UpdateStateElapsedTime(elapsedTime);

	}

	void MoveState::Finalize()
	{
		owner_->SetMoveVec({});	//	移動をやめる
	}

	void MoveState::DrawDebug()
	{
		if (ImGui::TreeNode("MoveState"))
		{
			ImGui::DragFloat("ElapsedTime", &stateElapsedTime_, 0.1f);

			ImGui::TreePop();
		}
	}

}

//	追跡ステート
namespace DroneState
{
	void PursuitState::Initialize()
	{

	}

	void PursuitState::Update(const float& elapsedTime)
	{
		//	----- 経過時間更新 -----
		UpdateStateElapsedTime(elapsedTime);

		//	----- 追跡処理 -----
		//	プレイヤーが索敵範囲外なら待機ステートへ遷移
		if (owner_->CalcDistanceToTarget() > owner_->GetSearchRange())
		{
			owner_->SetMoveVec({});							//	移動をやめる
			owner_->ChangeState(Drone::StateType::Idle);	//	待機ステートへ遷移
			return;
		}

		//	プレイヤーが射程範囲内なら攻撃ステートへ遷移
		if (owner_->CalcDistanceToTarget() < owner_->GetLaunchRange())
		{
			owner_->SetMoveVec({});							//	移動をやめる
			owner_->ChangeState(Drone::StateType::Attack);	//	攻撃ステートへ遷移
			return;
		}

		//	プレイヤーが索敵範囲内かつ、射程範囲外なら近づく
		owner_->ApproachingTarget(elapsedTime);

	}

	void PursuitState::Finalize()
	{
		owner_->SetMoveVec({});	//	移動をやめる
	}

	void PursuitState::DrawDebug()
	{
		if (ImGui::TreeNode("PursuitState"))
		{
			ImGui::DragFloat("ElapsedTime", &stateElapsedTime_, 0.1f);

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
		//	----- 経過時間更新 -----
		UpdateStateElapsedTime(elapsedTime);

		//	----- 弾丸処理 -----
		owner_->LaunchBullet();

		//	プレイヤーが射程範囲外に行ったら待機ステートへ遷移
		if (owner_->CalcDistanceToTarget() >= owner_->GetLaunchRange())
		{
			owner_->ChangeState(Drone::StateType::Idle);
		}

	}

	void AttackState::Finalize()
	{

	}

	void AttackState::DrawDebug()
	{
		if (ImGui::TreeNode("AttackState"))
		{
			ImGui::DragFloat("ElapsedTime", &stateElapsedTime_, 0.1f);

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
		//	----- 経過時間更新 -----
		UpdateStateElapsedTime(elapsedTime);

	}

	void AvoidanceState::Finalize()
	{

	}

	void AvoidanceState::DrawDebug()
	{
		if (ImGui::TreeNode("AvoidanceState"))
		{
			ImGui::DragFloat("ElapsedTime", &stateElapsedTime_, 0.1f);

			ImGui::TreePop();
		}
	}

}