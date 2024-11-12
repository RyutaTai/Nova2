#include "PlayerState.h"

#include <algorithm>

#include "../Nova/Debug/DebugRenderer.h"
#include "../Nova/Graphics/Graphics.h"
#include "../Nova/Collision/Collision.h"
#include "../Nova/Others/MathHelper.h"
#include "BulletManager.h"
#include "EnemyManager.h"
#include "../Nova/Input/GamePad.h"
#include "../Nova/Input/Input.h"

//	待機ステート
namespace PlayerState
{
	void IdleState::Initialize()
	{
		//	アニメーションセット
		owner_->PlayAnimation(Player::AnimationType::Idle, true, 0.2f);
		owner_->SetAnimationSpeed(1.0f);
	}

	void IdleState::Update(const float& elapsedTime)
	{
		//	移動入力があれば、移動ステートへ遷移
		if (owner_->InputMove(elapsedTime))
		{
			owner_->ChangeState(Player::StateType::Move);
			//owner_->PlayAnimation(Player::AnimationType::ANIM_WALK);
		 return;
		}
		
		//	攻撃ステートへ遷移
		if (owner_->GetButtonDown(GamePad::BTN_B/*Xキー*/))
		{
			owner_->ChangeState(Player::StateType::ComboOne1);
			return;
		}

	}

	void IdleState::Finalize()
	{
		owner_->SetAnimationSpeed(1.0f);
	}

	void IdleState::DrawDebug()
	{
		if (ImGui::TreeNode("Idle"))
		{

		}
	}
}

//	移動ステート
namespace PlayerState
{
	void MoveState::Initialize()
	{
		//	アニメーションセット
		owner_->PlayAnimation(Player::AnimationType::Run, true, 0.25f);
		owner_->SetAnimationSpeed(1.2f);

		owner_->SetMoveSpeed(4.0f);

	}

	void MoveState::Update(const float& elapsedTime)
	{
		//	移動入力がなくなったら待機ステートへ遷移
		if (!owner_->InputMove(elapsedTime))
		{
			//	待機ステートへ遷移
			owner_->GetStateMachine()->ChangeState(static_cast<int>(Player::StateType::Idle));
			return;
		}
		
	}

	void MoveState::Finalize()
	{
		owner_->SetMoveSpeed(2.0f);
		owner_->SetAnimationSpeed(1.0f);
	}

	void MoveState::DrawDebug()
	{
		if (ImGui::TreeNode("Move"))
		{

		}
	}
}

//	攻撃ステート
namespace PlayerState
{
	void AttackState::Initialize()
	{
		//	アニメーションセット
		//owner_->PlayAnimation(Player::AnimationType::Combo0_1, false, 2.0f, 0.0f);
		owner_->PlayAnimation(Player::AnimationType::Combo0_1, false, 0.0f);
		owner_->SetAnimationSpeed(1.0f);

		//	当たり判定タイマー初期化
		judgeTimer_ = 0.0f;
	}

	void AttackState::Update(const float& elapsedTime)
	{
#if 1
		//	アニメーション再生が終わったら待機ステートへ遷移
		if (owner_->IsPlayAnimation() == false /* && isMove_ == false*/)
		{
			owner_->GetStateMachine()->ChangeState(static_cast<int> (Player::StateType::Idle));
			return;
		}
#endif
		//	攻撃処理
		//PunchAttack(elapsedTime, "SKM_Manny_LOD0", "ik_hand_gun");

		//	判定用タイマー更新
		UpdateJudgeTimer(elapsedTime);

	}

	//	殴打攻撃（普通の攻撃）
	
	//bool AttackState::PunchAttack(const float& elapsedTime, const std::string& meshName, const std::string& boneName)
	//{
	//	bool isHIt = false;
	//
	//	//	右手のワールド座標取得
	//	DirectX::XMFLOAT4X4 world;
	//	DirectX::XMStoreFloat4x4(&world, owner_->GetTransform()->CalcWorld());	//	プレイヤーのワールド行列
	//	//DirectX::XMStoreFloat4x4(&world, owner_->GetTransform()->CalcWorldMatrix(scale));	//	プレイヤーのワールド行列
	//	DirectX::XMFLOAT3 leftHandPos = owner_->GetJointPosition(meshName, boneName, world);
	//
	//	//	当たり判定用の半径セット
	//	float leftHandRadius = 3.0f;
	//
	//	//	衝突判定用のデバッグ球を描画
	//	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
	//	//debugRenderer->DrawSphere(leftHandPos, leftHandRadius, DirectX::XMFLOAT4(1, 1, 1, 1));
	//
	//	//	弾丸への攻撃判定
	//	if (PuchVsBullet(elapsedTime, leftHandPos, leftHandRadius) == true)isHIt = true;
	//
	//	//	敵への当たり判定
	//	if (PunchVsEnemy(elapsedTime, leftHandPos, leftHandRadius) == true)isHIt = true;
	//
	//	return isHIt;
	//}

	//	拳と弾丸の当たり判定
	bool AttackState::PuchVsBullet(const float& elapsedTime, const DirectX::XMFLOAT3& leftHandPos, const float leftHandRadius)
	{
		bool isHitBullet = false;
		BulletManager& bulletManager = BulletManager::Instance();
		for (int bulletNum = 0; bulletNum < bulletManager.GetBulletCount(); ++bulletNum)
		{
			//	弾丸と右手との当たり判定
			Bullet* bullet = bulletManager.GetBullet(bulletNum);
			DirectX::XMFLOAT3	bulletPos = bullet->GetTransform()->GetPosition();	//	弾丸の位置
			float				bulletRadius = bullet->GetRadius();					//	弾丸の半径
			DirectX::XMFLOAT3	outPos = {};
			if (Collision::IntersectSphereVsSphere(leftHandPos, leftHandRadius, bulletPos, bulletRadius, outPos))
			{
				isHitBullet = true;
			}

			//	弾丸に拳が当たっていたら
			if (isHitBullet)
			{
				//	弾丸のダメージフラグ設定
				bulletManager.GetBullet(bulletNum)->SetDamaged(isHitBullet);

				// TODO:弾丸を打ってきた敵の方へ進む処理
				SetTargetPosition(bullet->GetOwnerPosition());
				MoveTowardsEnemy(elapsedTime);

			}
		}
		return isHitBullet;

	}

	//	敵の方向へに向かって移動
	void AttackState::MoveTowardsEnemy(const float& elapsedTime)
	{
		isMove_ = true;
		DirectX::XMVECTOR Velocity = {};
		DirectX::XMVECTOR PlayerPos = DirectX::XMLoadFloat3(&owner_->GetTransform()->GetPosition());	//	プレイヤーの位置
		DirectX::XMVECTOR TargetPos = DirectX::XMLoadFloat3(&targetPos_);								//	敵の位置
		DirectX::XMVECTOR Move = DirectX::XMVectorSubtract(TargetPos, PlayerPos);						//	プレイヤーから敵に向かうベクトル

		//	XZ平面のみの移動にする
		Move = DirectX::XMVectorSetY(Move, 0.0f);

		//	だんだんと移動する
		float moveTimer = moveTime_;
		Velocity = DirectX::XMVectorLerp(Velocity, Move, moveTimer);

		//	移動タイマー更新
		moveTimer -= elapsedTime;
		if (moveTimer < 0.0f)
		{
			isMove_ = false;
		}
		DirectX::XMFLOAT3 velocity;
		DirectX::XMStoreFloat3(&velocity, Velocity);

		//	プレイヤーのベロシティに加算
		//owner_->AddVelocity(move);
		owner_->SetVelocity(velocity);

		//	移動処理
		//owner_->Move();

	}

	void AttackState::Finalize()
	{
		judgeTimer_ = 0.0f;
		owner_->SetAnimationSpeed(1.0f);
	}

	void AttackState::DrawDebug()
	{
		if (ImGui::TreeNode("Attack"))
		{

		}
	}

}

//	コンボ01_1(右パンチ)
namespace PlayerState
{
	void ComboOne1::Initialize()
	{
		//	アニメーションセット
		owner_->PlayAnimation(Player::AnimationType::Combo0_1, false, 0.0f);
		owner_->SetAnimationSpeed(1.0f);

		//	ルートモーション
		owner_->SetUseRootMotion(true);

		//	判定時間セット
		animJudgeTime_.SetJudgeTime(0.55f, 0.735f);			//	アニメーション判定区間
		acceptInputFrame_ = 10.0f;							//	先行入力受付フレーム
		cancellationTime_.SetJudgeTime(0.3f, 1.16f);		//	キャンセル可能時間

		//	アニメーション速度変化区間セット
		animSpeedChangeInterval_[0].SetJudgeTime(0.0f, 0.32f);		//	パンチ前
		animSpeedChangeInterval_[1].SetJudgeTime(0.32f, 0.67f);		//	パンチ
		animSpeedChangeInterval_[2].SetJudgeTime(0.67f, 1.167f);	//	パンチ後

		//	キー入力判定初期化
		isCorrectInput_ = false;

		//	ステート経過時間初期化
		stateElapsedTime_ = 0.0f;

	}

	void ComboOne1::Update(const float& elapsedTime)
	{
		UpdateElapsedTime(elapsedTime);	//	経過時間更新
		UpdateAnimationSpeed();			//	アニメーション速度更新

		// TODO:アニメーションの長さ調整

		Command command = { KeyK };	//	入力判定
		JudgeInput(cancellationTime_, command);

		IsHit(elapsedTime, animJudgeTime_, "ik_hand_r");

		if (isCorrectInput_)	//	入力判定がtrueなら
		{
			owner_->ChangeState(Player::StateType::ComboOne2);
			isCorrectInput_ = false;
			return;
		}
		if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::Idle);
			return;
		}
	}

	//	animJudgeTimeに収まっていて、攻撃がヒットしているか判断する
	bool ComboOne1::IsHit(const float& elapsedTime, const JudgeTime& animJudgeTime, const std::string& nodeName)
	{
		//	時間での判定
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();
		if (animJudgeTime.IsJudgeFlag(currentAnimationSeconds) == false)
			return false;

		//	ノードと、敵または弾丸との当たり判定当たり判定
		if (owner_->JointVsEnemiesAndBullet(elapsedTime, nodeName, 5.0f) == false)	//	当たっていなかったらコンボキャンセル
		{
			//owner_->ChangeState(Player::StateType::Idle);
			return false;
		}

		return true;
	}

	void ComboOne1::JudgeInput(const JudgeTime& cancellationTime, const Command& command)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			isCorrectInput_ = true;
			return;
		}

		if (cancellationTime.IsJudgeFlag(stateElapsedTime_) == false)return;
		
		if (Input::Instance().CommandConfirm(command, acceptInputFrame_))
		{
			isCorrectInput_ = true;
			return;
		}

		return;
	}

	void ComboOne1::UpdateElapsedTime(const float& elapsedTime)
	{
		stateElapsedTime_ += elapsedTime;
	}

	//	アニメーション速度の微調整
	void ComboOne1::UpdateAnimationSpeed()
	{
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();	//	アニメーション再生時間

		if(animSpeedChangeInterval_[0].IsJudgeFlag(currentAnimationSeconds))
			owner_->SetAnimationSpeed(2.5f);
		else if(animSpeedChangeInterval_[1].IsJudgeFlag(currentAnimationSeconds))
			owner_->SetAnimationSpeed(1.5f);
		else if(animSpeedChangeInterval_[2].IsJudgeFlag(currentAnimationSeconds))
			owner_->SetAnimationSpeed(1.1f);
	}

	void ComboOne1::Finalize()
	{
		owner_->SetUseRootMotion(false);
		owner_->SetAnimationSpeed(1.0f);
	}

	void ComboOne1::DrawDebug()
	{
		if (ImGui::TreeNode("ComboOne1"))
		{
			ImGui::DragFloat("AcceptFrame", &acceptInputFrame_);		//	入力受付フレーム
			
			float cancellationTimeMin = cancellationTime_.GetMinTime();	//	キャンセル可能時間
			float cancellationTimeMax = cancellationTime_.GetMaxTime();
			ImGui::DragFloat("CancelTimeMin", &cancellationTimeMin);
			ImGui::DragFloat("CancelTimeMin", &cancellationTimeMax);
			cancellationTime_.SetJudgeTime(cancellationTimeMin, cancellationTimeMax);

		}
	}

}

//	コンボ01_2
namespace PlayerState
{
	void ComboOne2::Initialize()
	{
		//	アニメーションセット
		owner_->PlayAnimation(Player::AnimationType::Combo0_2, false, 0.0f);
		owner_->SetAnimationSpeed(1.0f);

		//	ルートモーション
		owner_->SetUseRootMotion(true);

		//	判定時間セット
		animJudgeTime_[0].SetJudgeTime(0.21f, 0.25f);
		animJudgeTime_[1].SetJudgeTime(0.29f, 0.63f);
		acceptInputFrame_ = 10.0f;
		cancellationTime_.SetJudgeTime(0.64f, 1.617f);

		//	アニメーション再生速度変化区間セット
		animSpeedChangeInterval_[0].SetJudgeTime(0.0f, 0.23f);		//	左パンチ出すまで
		animSpeedChangeInterval_[1].SetJudgeTime(0.24f, 0.55f);		//	左パンチからアッパー
		animSpeedChangeInterval_[2].SetJudgeTime(0.56f, 1.3f);		//	アッパーから構え
		animSpeedChangeInterval_[3].SetJudgeTime(1.1f, 1.617f);		//	構えから待機に戻る

		//	キー入力判定初期化
		isCorrectInput_ = false;

		//	ステート経過時間初期化
		stateElapsedTime_ = 0.0f;

	}

	void ComboOne2::Update(const float& elapsedTime)
	{
		UpdateElapsedTime(elapsedTime);	//	経過時間更新
		Command command = { KeyK };		//	入力判定
		JudgeInput(cancellationTime_, command);
		IsHit(elapsedTime, animJudgeTime_[0], "ik_hand_l");
		IsHit(elapsedTime, animJudgeTime_[1], "ik_hand_r");

		if (isCorrectInput_)	//	入力判定がtrueならコンボを進める
		{
			owner_->ChangeState(Player::StateType::ComboOne3);
			return;
		}
		if (owner_->IsPlayAnimation() == false)	//	アニメーション再生が終わったら待機へ遷移
		{
			owner_->ChangeState(Player::StateType::Idle);
			return;
		}

	}

	bool ComboOne2::IsHit(const float& elapsedTime, const JudgeTime& animJudgeTime, const std::string& nodeName)
	{
		//	時間での判定
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();
		if (animJudgeTime.IsJudgeFlag(currentAnimationSeconds) == false)
			return false;

		//	ノードと、敵または弾丸との当たり判定当たり判定
		if (owner_->JointVsEnemiesAndBullet(elapsedTime, nodeName, 5.0f) == false)	//	当たっていなかったらコンボキャンセル
		{
			//owner_->ChangeState(Player::StateType::Idle);
			return false;
		}

		return true;
	}

	void ComboOne2::JudgeInput(const JudgeTime& cancellationTime, const Command& command)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			isCorrectInput_ = true;
			return;
		}

		if (cancellationTime.IsJudgeFlag(stateElapsedTime_) == false)return;

		if (Input::Instance().CommandConfirm(command, acceptInputFrame_))
		{
			isCorrectInput_ = true;
			return;
		}
		return;
	}

	void ComboOne2::UpdateElapsedTime(const float& elapsedTime)
	{
		stateElapsedTime_ += elapsedTime;
	}

	//	アニメーション再生速度の微調整
	void ComboOne2::UpdateAnimationSpeed()
	{
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();

		if (animSpeedChangeInterval_[0].IsJudgeFlag(currentAnimationSeconds))
			owner_->SetAnimationSpeed(1.2f);
		else if (animSpeedChangeInterval_[1].IsJudgeFlag(currentAnimationSeconds))
			owner_->SetAnimationSpeed(1.1f);
		else if (animSpeedChangeInterval_[2].IsJudgeFlag(currentAnimationSeconds))
			owner_->SetAnimationSpeed(1.5f);
		else if (animSpeedChangeInterval_[3].IsJudgeFlag(currentAnimationSeconds))
			owner_->SetAnimationSpeed(1.5f);
	}

	void ComboOne2::Finalize()
	{
		owner_->SetUseRootMotion(false);
		owner_->SetAnimationSpeed(1.0f);
	}

	void ComboOne2::DrawDebug()
	{
		if (ImGui::TreeNode("ComboOne2"))
		{
			ImGui::DragFloat("AcceptFrame", &acceptInputFrame_);		//	入力受付フレーム

			float cancellationTimeMin = cancellationTime_.GetMinTime();	//	キャンセル可能時間
			float cancellationTimeMax = cancellationTime_.GetMaxTime();
			ImGui::DragFloat("CancelTimeMin", &cancellationTimeMin);
			ImGui::DragFloat("CancelTimeMin", &cancellationTimeMax);
			cancellationTime_.SetJudgeTime(cancellationTimeMin, cancellationTimeMax);

		}
	}

}

//	コンボ01_3
namespace PlayerState
{
	void ComboOne3::Initialize()
	{
		//	アニメーションセット
		owner_->PlayAnimation(Player::AnimationType::Combo0_3, false, 0.0f);
		owner_->SetAnimationSpeed(1.0f);

		//	ルートモーション
		owner_->SetUseRootMotion(true);

		//	判定時間セット
		animJudgeTime_[0].SetJudgeTime(0.07f, 0.127f);
		animJudgeTime_[1].SetJudgeTime(0.25f, 0.35f);
		animJudgeTime_[2].SetJudgeTime(0.53f, 0.76f);
		acceptInputFrame_ = 10.0f;
		cancellationTime_.SetJudgeTime(0.7f, 1.6f);

		//	キー入力判定初期化
		isCorrectInput_ = false;

		//	ステート経過時間初期化
		stateElapsedTime_ = 0.0f;

	}

	void ComboOne3::Update(const float& elapsedTime)
	{
		UpdateElapsedTime(elapsedTime);	//	経過時間更新
		Command command = { KeyK };		//入力判定
		JudgeInput(cancellationTime_, command);
		IsHit(elapsedTime, animJudgeTime_[0], "ik_hand_l");
		IsHit(elapsedTime, animJudgeTime_[1], "ik_hand_r");
		IsHit(elapsedTime, animJudgeTime_[2], "ik_foot_l");

		if (isCorrectInput_)	//	入力判定がtrueなら
		{
			owner_->ChangeState(Player::StateType::ComboOne4);
			return;
		}
		if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::Idle);
			return;
		}
	}

	bool ComboOne3::IsHit(const float& elapsedTime, const JudgeTime& animJudgeTime, const std::string& nodeName)
	{
		//	時間での判定
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();
		if (animJudgeTime.IsJudgeFlag(currentAnimationSeconds) == false)
			return false;

		//	ノードと、敵または弾丸との当たり判定当たり判定
		if (owner_->JointVsEnemiesAndBullet(elapsedTime, nodeName, 5.0f) == false)	//	当たっていなかったらコンボキャンセル
		{
			//owner_->ChangeState(Player::StateType::Idle);
			return false;
		}

		return true;
	}

	void ComboOne3::JudgeInput(const JudgeTime& cancellationTime, const Command& command)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			isCorrectInput_ = true;
			return;
		}

		if (cancellationTime.IsJudgeFlag(stateElapsedTime_) == false)return;

		//	判定時間内に指定したボタンが押されていたらisCorrectInput_をtrueにする
		if (Input::Instance().CommandConfirm(command, acceptInputFrame_))
		{
			isCorrectInput_ = true;
		}
	}

	void ComboOne3::UpdateElapsedTime(const float& elapsedTime)
	{
		stateElapsedTime_ += elapsedTime;
	}

	void ComboOne3::Finalize()
	{
		owner_->SetUseRootMotion(false);
		owner_->SetAnimationSpeed(1.0f);
	}

	void ComboOne3::DrawDebug()
	{
		if (ImGui::TreeNode("ComboOne3"))
		{
			ImGui::DragFloat("AcceptFrame", &acceptInputFrame_);		//	入力受付フレーム

			float cancellationTimeMin = cancellationTime_.GetMinTime();	//	キャンセル可能時間
			float cancellationTimeMax = cancellationTime_.GetMaxTime();
			ImGui::DragFloat("CancelTimeMin", &cancellationTimeMin);
			ImGui::DragFloat("CancelTimeMin", &cancellationTimeMax);
			cancellationTime_.SetJudgeTime(cancellationTimeMin, cancellationTimeMax);

		}
	}

}

//	コンボ01_4
namespace PlayerState
{
	void ComboOne4::Initialize()
	{
		//	アニメーションセット
		owner_->PlayAnimation(Player::AnimationType::Combo0_4, false, 0.0f);
		owner_->SetAnimationSpeed(1.0f);

		//	ルートモーション
		owner_->SetUseRootMotion(true);

		//	判定時間セット
		animJudgeTime_.SetJudgeTime(0.7f, 0.83f);
		acceptInputFrame_ = 10.0f;

		//	キー入力判定初期化
		isCorrectInput_ = false;

		//	ステート経過時間初期化
		stateElapsedTime_ = 0.0f;

	}

	void ComboOne4::Update(const float& elapsedTime)
	{
		UpdateElapsedTime(elapsedTime);	//	経過時間更新
		Command command = { KeyK };		//	入力判定
		JudgeInput(cancellationTime_, command);
		IsHit(elapsedTime, animJudgeTime_, "ik_hand_r");
		
		if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::Idle);
			return;
		}

	}

	bool ComboOne4::IsHit(const float& elapsedTime, const JudgeTime& animJudgeTime, const std::string& nodeName)
	{
		//	時間での判定
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();
		if (animJudgeTime.IsJudgeFlag(currentAnimationSeconds) == false)
			return false;

		//	ノードと、敵または弾丸との当たり判定当たり判定
		if (owner_->JointVsEnemiesAndBullet(elapsedTime, nodeName, 5.0f) == false)	//	当たっていなかったらコンボキャンセル
		{
			//owner_->ChangeState(Player::StateType::Idle);
			return false;
		}

		return true;
	}

	void ComboOne4::JudgeInput(const JudgeTime& cancellationTime, const Command& command)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			isCorrectInput_ = true;
			return;
		}

		if (cancellationTime.IsJudgeFlag(stateElapsedTime_) == false)return;

		//	判定時間内に指定したボタンが押されていたらisCorrectInput_をtrueにする
		if (Input::Instance().CommandConfirm(command, acceptInputFrame_))
		{
			isCorrectInput_ = true;
		}
	}

	void ComboOne4::UpdateElapsedTime(const float& elapsedTime)
	{
		stateElapsedTime_ += elapsedTime;
	}

	void ComboOne4::Finalize()
	{
		owner_->SetUseRootMotion(false);
		owner_->SetAnimationSpeed(1.0f);
	}

	void ComboOne4::DrawDebug()
	{
		if (ImGui::TreeNode("ComboOne4"))
		{
			ImGui::DragFloat("AcceptFrame", &acceptInputFrame_);		//	入力受付フレーム

			float cancellationTimeMin = cancellationTime_.GetMinTime();	//	キャンセル可能時間
			float cancellationTimeMax = cancellationTime_.GetMaxTime();
			ImGui::DragFloat("CancelTimeMin", &cancellationTimeMin);
			ImGui::DragFloat("CancelTimeMin", &cancellationTimeMax);
			cancellationTime_.SetJudgeTime(cancellationTimeMin, cancellationTimeMax);

		}
	}
}

//	回避ステート
namespace PlayerState
{
	void DodgeState::Initialize()
	{
		owner_->PlayAnimation(Player::AnimationType::DodgeFront, false, 0.0f);
		owner_->SetAnimationSpeed(1.0f);
	}

	void DodgeState::Update(const float& elapsedTime)
	{

	}

	void DodgeState::Finalize()
	{
		owner_->SetAnimationSpeed(1.0f);
	}

	void DodgeState::DrawDebug()
	{
		if (ImGui::TreeNode("Dodge"))
		{

		}
	}
}
