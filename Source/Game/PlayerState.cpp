#include "PlayerState.h"

#include <algorithm>

#include "Bullet.h"
#include "BulletManager.h"
#include "EnemyManager.h"
#include "Rhythm.h"
#include "../Nova/Debug/DebugRenderer.h"
#include "../Nova/Graphics/Graphics.h"
#include "../Nova/Collision/Collision.h"
#include "../Nova/Others/MathHelper.h"
#include "../Nova/Input/GamePad.h"
#include "../Nova/Input/Input.h"
#include "../Nova/Audio/AudioManager.h"
#include "../Nova/Scenes/SceneManager.h"
#include "../Nova/Collision/CollisionManager.h"

//	待機ステート
namespace PlayerState
{
	void IdleState::Initialize()
	{
		//	アニメーションセット
		//owner_->PlayAnimation(Player::AnimationType::Idle, true, 0.2f);
		owner_->PlayAnimation(Player::AnimationType::Idle, true, 0.3f);
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

			//	リズム判定処理
			Rhythm::Instance().GetJudgmentType(Rhythm::Instance().GetCurrentMidiTime(), elapsedTime);
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

			ImGui::TreePop();
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

		owner_->SetMoveSpeed(3.0f);

		//	足音SE再生
		//AudioManager::Instance().GetAudioResource("PlayerFootsteps")->Play(false);

		//	足音タイマーリセット
		footStepsTimer_ = 0.0f;

	}

	void MoveState::Update(const float& elapsedTime)
	{
		//	ステート経過時間更新
		UpdateStateElapsedTime(elapsedTime);
		
		//	足音再生
		PlayFootsteps(elapsedTime);

		//	移動入力がなくなったら待機ステートへ遷移
		if (!owner_->InputMove(elapsedTime))
		{
			//	待機ステートへ遷移
			owner_->ChangeState(Player::StateType::Idle);
			return;
		}

		//	攻撃ステートへ遷移
		if (owner_->GetButtonDown(GamePad::BTN_B/*Xキー*/))
		{
			owner_->ChangeState(Player::StateType::ComboOne1);

			//	リズム判定処理
			Rhythm::Instance().GetJudgmentType(Rhythm::Instance().GetCurrentMidiTime(), elapsedTime);
			return;
		}
		
	}

	//	足音SE再生
	void MoveState::PlayFootsteps(const float& elapsedTime)
	{
		//	ピッチを0.4～0.6の間でランダムに決める
#if 1
		srand(static_cast<unsigned int>(time(NULL)));
		float pitch = (rand() % 20 + 40) / 100.0f;
#else
		float pitch = Mathf::RandomRange(0.4f, 0.6f);	//	重い
#endif
		//	再生間隔に達していて、SEの再生も終わっていたら再生する
		if (footStepsTimer_ > playFootstepsInterval_)
		{
			footStepsTimer_ = 0.0f;
			AudioManager::Instance().GetAudioResource("PlayerFootsteps")->SetPitch(pitch);	//	ピッチ設定
			AudioManager::Instance().GetAudioResource("PlayerFootsteps")->Play(false);		//	再生

		}

		//	足音再生間隔更新
		footStepsTimer_ += elapsedTime;

	}

	void MoveState::Finalize()
	{
		owner_->SetMoveSpeed(2.0f);
		owner_->SetAnimationSpeed(1.0f);

		//	足音停止
		//AudioManager::Instance().GetAudioResource("PlayerFootsteps")->Stop();

		//	足音タイマーリセット
		footStepsTimer_ = 0.0f;

	}

	void MoveState::DrawDebug()
	{
		if (ImGui::TreeNode("Move"))
		{
			ImGui::DragFloat("PlayFootstepsInterval", &playFootstepsInterval_, 0.01f);	//	足音SE再生間隔
			ImGui::DragFloat("FootstepsTimer", &footStepsTimer_, 0.01f);				//	足音SE再生間隔タイマー

			bool isPlaying = AudioManager::Instance().GetAudioResource("PlayerFootsteps")->IsPlaying();
			ImGui::Checkbox("IsPlaying", &isPlaying);

			ImGui::TreePop();
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
			owner_->ChangeState(Player::StateType::Idle);
			return;
		}
#endif
		//	判定用タイマー更新
		UpdateJudgeTimer(elapsedTime);

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

			ImGui::TreePop();
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
		animJudgeTime_.SetJudgeTime(0.180f, 0.38f);			//	アニメーション判定区間
		//acceptInputFrame_ = 10.0f;						//	先行入力受付フレーム
		cancellationTime_.SetJudgeTime(0.3f, 1.16f);		//	キャンセル可能時間

		//	アニメーション速度変化区間セット
		animSpeedChangeInterval_[0].SetJudgeTime(0.0f, 0.32f);		//	パンチ前
		animSpeedChangeInterval_[1].SetJudgeTime(0.32f, 0.67f);		//	パンチ
		animSpeedChangeInterval_[2].SetJudgeTime(0.67f, 1.167f);	//	パンチ後

		//	ステート経過時間初期化
		stateElapsedTime_ = 0.0f;

		//	プレイヤーの攻撃判定を無効にする
		owner_->SetAllAttackDetectionActiveFlag(false);
		owner_->SetAttackHit(false);

		//	攻撃中は押し出し判定しない
		owner_->SetIsActiveCollisionDetection(false);

		//	プレイヤーのコンボ数を初期化
		owner_->ResetComboCount();

	}

	void ComboOne1::Update(const float& elapsedTime)
	{
		//	経過時間更新
		UpdateStateElapsedTime(elapsedTime);

		//	アニメーション速度更新
		UpdateAnimationSpeed();			

		//	プレイヤーの攻撃判定を有効にする
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();	//	アニメーション再生時間
		if (animJudgeTime_.IsJudgeFlag(currentAnimationSeconds))
		{
			owner_->GetAttackDetectionData("RightPunch").SetIsActive(true);
		}

		//	次のステートへの遷移
		if (JudgeInput(cancellationTime_))	
		{
			//	リズム判定処理(missならreturn)
			if (Rhythm::Instance().GetJudgmentType(Rhythm::Instance().GetCurrentMidiTime(), elapsedTime) == Rhythm::JudgmentType::Miss)
			return;

			//	miss以外なら次のステートへ遷移
			owner_->ChangeState(Player::StateType::ComboOne2);
			return;
		}
		if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::Idle);
			return;
		}
	}

	bool ComboOne1::JudgeInput(const JudgeTime& cancellationTime)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			return true;
		}

		//if (cancellationTime.IsJudgeFlag(stateElapsedTime_) == false)return false;

		if (owner_->GetButtonDown(GamePad::BTN_B/*Xキー*/))
		{
			return true;
		}

		return false;
	}

	bool ComboOne1::JudgeInputCommand(const JudgeTime& cancellationTime, const Command& command)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			return true;
		}

		if (cancellationTime.IsJudgeFlag(stateElapsedTime_) == false)return false;
		
		if (Input::Instance().CommandConfirm(command, acceptInputFrame_))
		{
			return true;
		}

		return false;
	}

	void ComboOne1::UpdateStateElapsedTime(const float& elapsedTime)
	{
		//stateElapsedTime_ += elapsedTime;
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();	//	アニメーション再生時間
		stateElapsedTime_ = currentAnimationSeconds;
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

		//	プレイヤーの攻撃判定を無効にする
		owner_->SetAllAttackDetectionActiveFlag(false);
		//	プレイヤーの押し出し判定を有効化
		owner_->SetIsActiveCollisionDetection(true);

	}

	void ComboOne1::DrawDebug()
	{
		if (ImGui::TreeNode("ComboOne1"))
		{
			ImGui::DragFloat("StateElapsedTime", &stateElapsedTime_);	//	ステート経過時間
			ImGui::DragFloat("AcceptFrame", &acceptInputFrame_);		//	入力受付フレーム
			
			float cancellationTimeMin = cancellationTime_.GetMinTime();	//	キャンセル可能時間
			float cancellationTimeMax = cancellationTime_.GetMaxTime();
			ImGui::DragFloat("CancelTimeMin", &cancellationTimeMin);
			ImGui::DragFloat("CancelTimeMin", &cancellationTimeMax);
			cancellationTime_.SetJudgeTime(cancellationTimeMin, cancellationTimeMax);

			ImGui::TreePop();
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
		animJudgeTime_[0].SetJudgeTime(0.21f, 0.25f);	//	アニメーション再生中に当たっているか判定(アニメーション再生時間をもとに判定)
		animJudgeTime_[1].SetJudgeTime(0.29f, 0.63f);
		acceptInputFrame_ = 10.0f;
		cancellationTime_.SetJudgeTime(0.64f, 1.617f);	//	入力判定に使用

		//	アニメーション再生速度変化区間セット
		animSpeedChangeInterval_[0].SetJudgeTime(0.0f, 0.23f);		//	左パンチ出すまで
		animSpeedChangeInterval_[1].SetJudgeTime(0.24f, 0.55f);		//	左パンチからアッパー
		animSpeedChangeInterval_[2].SetJudgeTime(0.56f, 1.3f);		//	アッパーから構え
		animSpeedChangeInterval_[3].SetJudgeTime(1.1f, 1.617f);		//	構えから待機に戻る

		//	ステート経過時間初期化
		stateElapsedTime_ = 0.0f;

		//	プレイヤーの攻撃判定を無効にする
		owner_->SetAllAttackDetectionActiveFlag(false);
		owner_->SetAttackHit(false);
		//	攻撃中は押し出し判定しない
		owner_->SetIsActiveCollisionDetection(false);
	}

	void ComboOne2::Update(const float& elapsedTime)
	{
		//	経過時間更新
		UpdateStateElapsedTime(elapsedTime);	

		//	アニメーション速度更新
		UpdateAnimationSpeed();

		//	当たり判定処理(アニメーションが再生されたら再生時間をもとに判定する)
		//	一撃目
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();	//	アニメーション再生時間
		if (animJudgeTime_[0].IsJudgeFlag(currentAnimationSeconds))
			owner_->GetAttackDetectionData("LeftPunch").SetIsActive(true);

		//	一撃目のアニメーションが終わったらヒットフラグをオフにする
		if (animJudgeTime_[0].GetMaxTime() < currentAnimationSeconds && currentAnimationSeconds < animJudgeTime_[1].GetMinTime())
			owner_->SetAttackHit(false);

		//	二撃目
		if (animJudgeTime_[1].IsJudgeFlag(currentAnimationSeconds))
			owner_->GetAttackDetectionData("RightPunch").SetIsActive(true);

		//	次のステートへ遷移
		if (JudgeInput(cancellationTime_))	//	入力判定がtrueならコンボを進める
		{
			//	リズム判定処理(missならreturn)
			if (Rhythm::Instance().GetJudgmentType(Rhythm::Instance().GetCurrentMidiTime(), elapsedTime) == Rhythm::JudgmentType::Miss)
			return;

			//	次のステートへ遷移
			owner_->ChangeState(Player::StateType::ComboOne3);
			return;
		}

		//	アニメーション再生が終わったら待機へ遷移
		if (owner_->IsPlayAnimation() == false)	
		{
			owner_->ChangeState(Player::StateType::Idle);
			return;
		}

	}

	bool ComboOne2::JudgeInput(const JudgeTime& cancellationTime)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			return true;
		}

		//if (cancellationTime.IsJudgeFlag(stateElapsedTime_) == false)return false;

		if (owner_->GetButtonDown(GamePad::BTN_B/*Xキー*/))
		{
			return true;
		}

		return false;

	}

	bool ComboOne2::JudgeInputCommand(const JudgeTime& cancellationTime, const Command& command)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			return true;
		}

		if (cancellationTime.IsJudgeFlag(stateElapsedTime_) == false)return false;

		if (Input::Instance().CommandConfirm(command, acceptInputFrame_))
		{
			return true;
		}
		return false;
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

		//	プレイヤーの攻撃判定を無効にする
		owner_->SetAllAttackDetectionActiveFlag(false);
		//	プレイヤーの押し出し判定を有効化
		owner_->SetIsActiveCollisionDetection(true);
	}

	void ComboOne2::DrawDebug()
	{
		if (ImGui::TreeNode("ComboOne2"))
		{
			ImGui::DragFloat("StateElapsedTime", &stateElapsedTime_);	//	ステート経過時間
			ImGui::DragFloat("AcceptFrame", &acceptInputFrame_);		//	入力受付フレーム

			float cancellationTimeMin = cancellationTime_.GetMinTime();	//	キャンセル可能時間
			float cancellationTimeMax = cancellationTime_.GetMaxTime();
			ImGui::DragFloat("CancelTimeMin", &cancellationTimeMin);
			ImGui::DragFloat("CancelTimeMin", &cancellationTimeMax);
			cancellationTime_.SetJudgeTime(cancellationTimeMin, cancellationTimeMax);

			ImGui::TreePop();
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

		//	ステート経過時間初期化
		stateElapsedTime_ = 0.0f;

		//	プレイヤーの攻撃判定を無効にする
		owner_->SetAllAttackDetectionActiveFlag(false);
		owner_->SetAttackHit(false);
		//	攻撃中は押し出し判定しない
		owner_->SetIsActiveCollisionDetection(false);
	}

	void ComboOne3::Update(const float& elapsedTime)
	{
		//	経過時間更新
		UpdateStateElapsedTime(elapsedTime);

		//	一撃目の判定
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();	//	アニメーション再生時間
		if (animJudgeTime_[0].IsJudgeFlag(currentAnimationSeconds))
		{
			owner_->GetAttackDetectionData("LeftPunch").SetIsActive(true);
		}
		//	一撃目のアニメーションが終わったらヒットフラグをオフにする
		if (animJudgeTime_[0].GetMaxTime() < currentAnimationSeconds && currentAnimationSeconds < animJudgeTime_[1].GetMinTime())
			owner_->SetAttackHit(false);

		//	二撃目の判定
		if (animJudgeTime_[1].IsJudgeFlag(currentAnimationSeconds))
		{
			owner_->GetAttackDetectionData("RightPunch").SetIsActive(true);
		}
		//	二撃目のアニメーションが終わったらヒットフラグをオフにする
		if (animJudgeTime_[1].GetMaxTime() < currentAnimationSeconds && currentAnimationSeconds < animJudgeTime_[2].GetMinTime())
			owner_->SetAttackHit(false);

		//	三撃目の判定
		if (animJudgeTime_[2].IsJudgeFlag(currentAnimationSeconds))
		{
			owner_->GetAttackDetectionData("LeftKick").SetIsActive(true);
		}

		if (JudgeInput(cancellationTime_))	//	入力判定がtrueなら
		{
			//	リズム判定処理(missならreturn)
			if (Rhythm::Instance().GetJudgmentType(Rhythm::Instance().GetCurrentMidiTime(), elapsedTime) == Rhythm::JudgmentType::Miss)
			return;

			//	次のステートへ遷移
			owner_->ChangeState(Player::StateType::ComboOne4);
			return;
		}
		if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::Idle);
			return;
		}
	}

	bool ComboOne3::JudgeAttackHit(const float& elapsedTime, const JudgeTime& animJudgeTime, const std::string& nodeName)
	{
		//	時間での判定
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();
		if (animJudgeTime.IsJudgeFlag(currentAnimationSeconds) == false)
			return false;

		//	ノードと、敵または弾丸との当たり判定
		if (owner_->JointVsEnemiesAndBullet(elapsedTime, nodeName, 5.0f) == false)	//	当たっていなかったらコンボキャンセル
		{
			//owner_->ChangeState(Player::StateType::Idle);
			return false;
		}

		return true;
	}

	bool ComboOne3::JudgeInput(const JudgeTime& cancellationTime)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			return true;
		}

		//if (cancellationTime.IsJudgeFlag(stateElapsedTime_) == false)return false;

		if (owner_->GetButtonDown(GamePad::BTN_B/*Xキー*/))
		{
			return true;
		}

		return false;

	}

	bool ComboOne3::JudgeInputCommand(const JudgeTime& cancellationTime, const Command& command)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			return true;
		}

		if (cancellationTime.IsJudgeFlag(stateElapsedTime_) == false)return false;

		//	判定時間内に指定したボタンが押されていたらisCorrectInput_をtrueにする
		if (Input::Instance().CommandConfirm(command, acceptInputFrame_))
		{
			return true;
		}
	}

	void ComboOne3::Finalize()
	{
		owner_->SetUseRootMotion(false);
		owner_->SetAnimationSpeed(1.0f);

		//	プレイヤーの攻撃判定を無効にする
		owner_->SetAllAttackDetectionActiveFlag(false);
		//	プレイヤーの押し出し判定を有効化
		owner_->SetIsActiveCollisionDetection(true);
	}

	void ComboOne3::DrawDebug()
	{
		if (ImGui::TreeNode("ComboOne3"))
		{
			ImGui::DragFloat("StateElapsedTime", &stateElapsedTime_);	//	ステート経過時間
			ImGui::DragFloat("AcceptFrame", &acceptInputFrame_);		//	入力受付フレーム

			float cancellationTimeMin = cancellationTime_.GetMinTime();	//	キャンセル可能時間
			float cancellationTimeMax = cancellationTime_.GetMaxTime();
			ImGui::DragFloat("CancelTimeMin", &cancellationTimeMin);
			ImGui::DragFloat("CancelTimeMin", &cancellationTimeMax);
			cancellationTime_.SetJudgeTime(cancellationTimeMin, cancellationTimeMax);

			ImGui::TreePop();
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
		animJudgeTime_.SetJudgeTime(0.64f, 1.185f);
		acceptInputFrame_ = 10.0f;

		//	ステート経過時間初期化
		stateElapsedTime_ = 0.0f;

		//	プレイヤーの攻撃判定を無効にする
		owner_->SetAllAttackDetectionActiveFlag(false);
		owner_->SetAttackHit(false);
		//	攻撃中は押し出し判定しない
		owner_->SetIsActiveCollisionDetection(false);
	}

	void ComboOne4::Update(const float& elapsedTime)
	{
		//	経過時間更新
		UpdateStateElapsedTime(elapsedTime);
	
		//	リズム判定をとって判定文字を出すため
		Rhythm::Instance().GetJudgmentType(Rhythm::Instance().GetCurrentMidiTime(), elapsedTime);
		
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();	//	アニメーション再生時間
		if (animJudgeTime_.IsJudgeFlag(currentAnimationSeconds))
		{
			owner_->GetAttackDetectionData("RightPunch").SetIsActive(true);
		}
		
		if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::Idle);

			return;
		}

	}

	bool ComboOne4::JudgeAttackHit(const float& elapsedTime, const JudgeTime& animJudgeTime, const std::string& nodeName)
	{
		//	時間での判定
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();
		if (animJudgeTime.IsJudgeFlag(currentAnimationSeconds) == false)
			return false;

		//	ノードと、敵または弾丸との当たり判定
		if (owner_->JointVsEnemiesAndBullet(elapsedTime, nodeName, 5.0f) == false)	//	当たっていなかったらコンボキャンセル
		{
			//owner_->ChangeState(Player::StateType::Idle);
			return false;
		}

		return true;
	}

	bool ComboOne4::JudgeInput(const JudgeTime& cancellationTime)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			return true;
		}

		//if (cancellationTime.IsJudgeFlag(stateElapsedTime_) == false)return false;

		if (owner_->GetButtonDown(GamePad::BTN_B/*Xキー*/))
		{
			return true;
		}

		return false;
	}

	bool ComboOne4::JudgeInputCommand(const JudgeTime& cancellationTime, const Command& command)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			return true;
		}

		if (cancellationTime.IsJudgeFlag(stateElapsedTime_) == false)return false;

		//	判定時間内に指定したボタンが押されていたらisCorrectInput_をtrueにする
		if (Input::Instance().CommandConfirm(command, acceptInputFrame_))
		{
			return true;
		}

		return false;
	}

	void ComboOne4::Finalize()
	{
		owner_->SetUseRootMotion(false);
		owner_->SetAnimationSpeed(1.0f);

		//	プレイヤーの攻撃判定を無効にする
		owner_->SetAllAttackDetectionActiveFlag(false);
		//	プレイヤーの押し出し判定を有効化
		owner_->SetIsActiveCollisionDetection(true);
	}

	void ComboOne4::DrawDebug()
	{
		if (ImGui::TreeNode("ComboOne4"))
		{
			ImGui::DragFloat("StateElapsedTime", &stateElapsedTime_);	//	ステート経過時間
			ImGui::DragFloat("AcceptFrame", &acceptInputFrame_);		//	入力受付フレーム

			float cancellationTimeMin = cancellationTime_.GetMinTime();	//	キャンセル可能時間
			float cancellationTimeMax = cancellationTime_.GetMaxTime();
			ImGui::DragFloat("CancelTimeMin", &cancellationTimeMin);
			ImGui::DragFloat("CancelTimeMin", &cancellationTimeMax);
			cancellationTime_.SetJudgeTime(cancellationTimeMin, cancellationTimeMax);

			ImGui::TreePop();
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

			ImGui::TreePop();
		}
	}
}

//	ダメージステート
namespace PlayerState
{
	void DamageState::Initialize()
	{
		//	アニメーションの速度を変える区間を設定
		animSpeedChangeInterval_[0].SetJudgeTime(0.0f, 0.54f);		//	地面につく
		animSpeedChangeInterval_[1].SetJudgeTime(0.55f, 1.16f);		//	動作終わり
		animSpeedChangeInterval_[2].SetJudgeTime(1.17f, 2.2f);		//	余韻

		//	くらいモーション
		//owner_->PlayAnimation(Player::AnimationType::HitFront, false, 0.1f, 0.2f);
		//owner_->SetAnimationSpeed(1.8f);

		//	吹っ飛びモーション
		owner_->PlayAnimation(Player::AnimationType::HitDeath, false, 0.1f, 1.0f, 0.35f, 1.18f);
		owner_->SetAnimationSpeed(1.5f);

		//  敵からプレイヤーの方向へ吹っ飛ばす
		DirectX::XMFLOAT3 direction = Normalize(owner_->GetTransform()->GetPosition() - owner_->GetEnemyPos());
		direction.y = 0.0f;
		owner_->AddForce(direction, blowPower_, decelerationForce_);

		//	
		/*DirectX::XMFLOAT3 playerUp = owner_->GetTransform()->CalcUp();
		float angle = XMFLOAT3Dot(Normalize(playerUp), direction);
		owner_->GetTransform()->SetRotationY(angle);*/
	}

	void DamageState::Update(const float& elapsedTime)
	{
		//	ステート経過時間更新
		UpdateStateElapsedTime(elapsedTime);
		
		//	アニメーション速度調整
		UpdateAnimationSpeed();

		//	アニメーション再生が終わったらステート終了
		if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::Idle);
		}

	}

	void DamageState::UpdateAnimationSpeed()
	{
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();	//	アニメーション再生時間

		//	アニメーション速度更新
		for (int i = 0; i < AnimSpeedSectionCount; ++i)
		{
			if (animSpeedChangeInterval_[i].IsJudgeFlag(currentAnimationSeconds))
			{
				owner_->SetAnimationSpeed(animationSpeed_[i]);
				break;
			}
		}

	/*	if (animSpeedChangeInterval_[0].IsJudgeFlag(currentAnimationSeconds))
			owner_->SetAnimationSpeed(1.0f);
		else if (animSpeedChangeInterval_[1].IsJudgeFlag(currentAnimationSeconds))
			owner_->SetAnimationSpeed(1.2f);
		else if (animSpeedChangeInterval_[2].IsJudgeFlag(currentAnimationSeconds))
			owner_->SetAnimationSpeed(2.0f);*/
	}

	void DamageState::Finalize()
	{
		owner_->SetAnimationSpeed(1.0f);
		owner_->GetTransform()->SetRotationY(0.0f);
	}

	void DamageState::DrawDebug()
	{
		if (ImGui::TreeNode("Damage"))
		{
			if(ImGui::TreeNode("AnimationSpeed"))
			{
				ImGui::DragFloat("AnimSpeedSection0", &animationSpeed_[0]);
				ImGui::DragFloat("AnimSpeedSection1", &animationSpeed_[1]);
				ImGui::DragFloat("AnimSpeedSection2", &animationSpeed_[2]);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("AddForce"))
			{
				ImGui::DragFloat("BlowPower", &blowPower_);
				ImGui::DragFloat("DecelerationForce", &decelerationForce_);

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}
}

//	怯みステート
namespace PlayerState
{
	void FlinchState::Initialize()
	{

	}

	void FlinchState::Update(const float& elapsedTime)
	{
		owner_->ChangeState(Player::StateType::Idle);
	}

	void FlinchState::Finalize()
	{

	}

	void FlinchState::DrawDebug()
	{
		if (ImGui::TreeNode("Flinch"))
		{

			ImGui::TreePop();
		}
	}
}

//	死亡ステート
namespace PlayerState
{
	void DeathState::Initialize()
	{

	}

	void DeathState::Update(const float& elapsedTime)
	{
		owner_->ChangeState(Player::StateType::Idle);
	}

	void DeathState::Finalize()
	{

	}

	void DeathState::DrawDebug()
	{
		if (ImGui::TreeNode("Death"))
		{

			ImGui::TreePop();
		}
	}

}
