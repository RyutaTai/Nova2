#include "DragonkinAction.h"

#include "../Nova/Others/MathHelper.h"
#include "Player.h"

//	待機行動(IdleAction)
namespace DragonkinAction
{
	ActionBase::State IdleAction::Run(const float& elapsedTime)
	{
		float runTimer = owner_->GetRunTimer();
		switch (step_)
		{
		case 0:
			owner_->SetRunTimer(Mathf::RandomRange(3.0f, 5.0f));
			owner_->PlayAnimation(static_cast<int>(Dragonkin::AnimationType::Idle01), false, 0.2f);
			step_++;
			break;
		case 1:
			runTimer -= elapsedTime;
			//	タイマー更新
			owner_->SetRunTimer(runTimer);

			//	待機時間が過ぎた時
			if (runTimer <= 0.0f)
			{
				owner_->SetRandomTargetPosition();
				step_ = 0;
				return ActionBase::State::Complete;
			}

			//	プレイヤーを見つけた時
			if (owner_->SearchPlayer())
			{
				step_ = 0;
				return ActionBase::State::Complete;
			}
			break;
		}
		return ActionBase::State::Run;
	}

	void IdleAction::DrawDebug()
	{
		if (ImGui::TreeNode("IdleAction"))
		{

			ImGui::TreePop();
		}
	}

}

//	通常殴打(AttackPunchAction)
namespace DragonkinAction
{
	ActionBase::State AttackPunchAction::Run(const float& elapsedTime)
	{
		float runTimer = owner_->GetRunTimer();
		switch (step_)
		{
		case 0:
			//	アニメーション再生
			owner_->PlayAnimation(static_cast<int>(Dragonkin::AnimationType::AttackPunch), false, 0.2f);
			//	判定を取る区間を設定
			animJudgeTime_.SetJudgeTime(0.42f, 0.53f);

			step_++;
			break;
		case 1:
			//	自分の位置、ジョイントの位置
			DirectX::XMFLOAT4X4 world;
			DirectX::XMStoreFloat4x4(&world, owner_->GetTransform()->CalcWorld());	//	プレイヤーのワールド行列
			DirectX::XMFLOAT3 jointPos = owner_->GetJointPosition("Hand_R", world);
			float jointRadius = 5.0f;

			//	プレイヤーとの当たり判定
			if (owner_->JudgeAttackHit(elapsedTime, animJudgeTime_, jointPos, jointRadius))
			{
				Player::Instance().SubtractHp(5);
			}

			//	アニメーション再生が終わったら終了
			if (owner_->IsPlayAnimation() == false)
			{
				step_ = 0;
				return ActionBase::State::Complete;
			}

			break;
		}
		return ActionBase::State::Run;
	}

	void AttackPunchAction::DrawDebug()
	{
		if (ImGui::TreeNode("AttackPunchAction"))
		{

			ImGui::TreePop();
		}
	}

}

//	通常キック(AttackKickAction)
namespace DragonkinAction
{
	ActionBase::State AttackKickAction::Run(const float& elapsedTime)
	{
		float runTimer = owner_->GetRunTimer();
		switch (step_)
		{
		case 0:
			//	アニメーション再生
			owner_->PlayAnimation(static_cast<int>(Dragonkin::AnimationType::AttackKick), false, 0.2f);
			//	判定を取る区間を設定
			animJudgeTime_.SetJudgeTime(0.52f, 0.6f);
			
			step_++;
			break;
		case 1:
			//	自分の位置、ジョイントの位置
			DirectX::XMFLOAT4X4 world;
			DirectX::XMStoreFloat4x4(&world, owner_->GetTransform()->CalcWorld());	//	プレイヤーのワールド行列
			DirectX::XMFLOAT3 jointPos = owner_->GetJointPosition("Foot_L", world);
			float jointRadius = 5.0f;

			//	プレイヤーとの当たり判定
			if (owner_->JudgeAttackHit(elapsedTime, animJudgeTime_, jointPos, jointRadius))
			{
				Player::Instance().SubtractHp(5);
			}

			//	アニメーション再生が終わったら終了
			if (owner_->IsPlayAnimation() == false)
			{
				step_ = 0;
				return ActionBase::State::Complete;
			}

			break;
		}
		return ActionBase::State::Run;
	}

	void AttackKickAction::DrawDebug()
	{
		if (ImGui::TreeNode("AttackKickAction"))
		{

			ImGui::TreePop();
		}
	}

}

//	通常翼攻撃(AttackWingAction)
namespace DragonkinAction
{
	ActionBase::State AttackWingAction::Run(const float& elapsedTime)
	{
		float runTimer = owner_->GetRunTimer();
		switch (step_)
		{
		case 0:
			//	アニメーション再生
			owner_->PlayAnimation(static_cast<int>(Dragonkin::AnimationType::AttackWing), false, 0.2f);
			//	判定を取る区間を設定
			animJudgeTime_.SetJudgeTime(0.34f, 0.41f);
			
			step_++;
			break;
		case 1:
			//	自分の位置、ジョイントの位置
			DirectX::XMFLOAT4X4 world;
			DirectX::XMStoreFloat4x4(&world, owner_->GetTransform()->CalcWorld());	//	プレイヤーのワールド行列
			DirectX::XMFLOAT3 jointPos = owner_->GetJointPosition("Foot_L", world);
			float jointRadius = 5.0f;

			//	プレイヤーとの当たり判定
			if (owner_->JudgeAttackHit(elapsedTime, animJudgeTime_, jointPos, jointRadius))
			{
				Player::Instance().SubtractHp(5);
			}

			//	アニメーション再生が終わったら終了
			if (owner_->IsPlayAnimation() == false)
			{
				step_ = 0;
				return ActionBase::State::Complete;
			}

			break;
		}
		return ActionBase::State::Run;
	}

	void AttackWingAction::DrawDebug()
	{
		if (ImGui::TreeNode("AttackWingAction"))
		{

			ImGui::TreePop();
		}
	}

}

//	スキル攻撃行動(SkillAction)
namespace DragonkinAction
{
	ActionBase::State SkillAction::Run(const float& elapsedTime)
	{
		return ActionBase::State::Run;
	}

	void SkillAction::DrawDebug()
	{
		if (ImGui::TreeNode("SkillAction"))
		{

			ImGui::TreePop();
		}
	}
}

//	追跡行動(PursuitAction)
namespace DragonkinAction
{
	ActionBase::State PursuitAction::Run(const float& elapsedTime)
	{
		return ActionBase::State::Run;
	}

	void PursuitAction::DrawDebug()
	{
		if (ImGui::TreeNode("PursuitAction"))
		{

			ImGui::TreePop();
		}
	}

}

//	徘徊行動(WanderAction)
namespace DragonkinAction
{
	ActionBase::State WanderAction::Run(const float& elasedTime)
	{
		return ActionBase::State::Run;
	}

	void WanderAction::DrawDebug()
	{
		if (ImGui::TreeNode("WanderAction"))
		{

			ImGui::TreePop();
		}
	}

}

//	逃走行動(LeaveAction)
namespace DragonkinAction
{
	ActionBase::State LeaveAction::Run(const float& elapsedTime)
	{
		return ActionBase::State::Run;
	}

	void LeaveAction::DrawDebug()
	{
		if (ImGui::TreeNode("LeaveAction"))
		{

			ImGui::TreePop();
		}
	}

}

//	回復行動(RecoverAction)
namespace DragonkinAction
{
	ActionBase::State RecoverAction::Run(const float& elapsedTime)
	{
		return ActionBase::State::Run;
	}

	void RecoverAction::DrawDebug()
	{
		if (ImGui::TreeNode("RecoverAction"))
		{

			ImGui::TreePop();
		}
	}

}