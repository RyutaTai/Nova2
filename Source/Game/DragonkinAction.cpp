#include "DragonkinAction.h"

#include "../Nova/Others/MathHelper.h"

//	待機行動
namespace DragonkinAction
{
	ActionBase::State IdleAction::Run(const float& elapsedTime)
	{
		float runTimer = owner_->GetRunTimer();
		switch (step_)
		{
		case 0:
			owner_->SetRunTimer(Mathf::RandomRange(3.0f, 5.0f));
			owner_->PlayAnimation(Dragonkin::AnimationType::ANIM_IDLE01, true, 0.2f);
			step_++;
			break;
		//case 1:
		//	if (owner_->IsPlayAnimation() == false) step_ = 0;

		//	break;
		case 1:
			runTimer -= elapsedTime;
			// タイマー更新
			owner_->SetRunTimer(runTimer);

			// 待機時間が過ぎた時
			if (runTimer <= 0.0f)
			{
				owner_->SetRandomTargetPosition();
				step_ = 0;
				//return ActionBase::State::Complete;
			}

			// プレイヤーを見つけた時
			if (owner_->SearchPlayer())
			{
				step_ = 0;
				//return ActionBase::State::Complete;
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

//	通常攻撃
namespace DragonkinAction
{
	ActionBase::State NormalAction::Run(const float& elapsedTime)
	{
		return ActionBase::State::Run;
	}

	void NormalAction::DrawDebug()
	{
		if (ImGui::TreeNode("NormalAction"))
		{

			ImGui::TreePop();
		}
	}
}

//	スキル攻撃行動
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

//	追跡行動
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

//	徘徊行動
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

//	逃走行動
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

//	回復行動
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