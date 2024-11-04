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
		owner_->PlayAnimation(Player::AnimationType::Idle, true, 1.0f, 0.2f);
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

	}
}

//	移動ステート
namespace PlayerState
{
	void MoveState::Initialize()
	{
#if 0
		//	アニメーションセット
		owner_->PlayBlendAnimation(Player::AnimationType::ANIM_WALK, true);

		owner_->SetWeight(0.5f);
#endif
		//	アニメーションセット
		owner_->PlayAnimation(Player::AnimationType::Walk , true, 1.0f, 0.2f);

		//	更新処理に使う変数初期化
		walkTimer_ = 0.0f;
		velocityScale_ = 1.0f;
	}

	void MoveState::Update(const float& elapsedTime)
	{
#if 0
		//	アニメーションのweight値を加算
		owner_->AddWeight(elapsedTime);
#endif
		//	一定以上の時間が経過したら走りモーションへ移行
		walkTimer_ += walkTimerAdd_ * elapsedTime;
		//owner_->MultiplyVelocityXZ(velocityScale_, elapsedTime);
		if (walkTimer_ > walkoToRunInterval_ && owner_->GetCurrentAnimType() != Player::AnimationType::Run)
		{
			owner_->PlayAnimation(Player::AnimationType::Run, true, 1.0f, 0.2f);
			owner_->SetMoveSpeed(4.5f);	//	移動スピード切り替え
		}
		//else velocityScale_ += velocityAdd_ * elapsedTime;

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
	}
}

//	攻撃ステート
namespace PlayerState
{
	void AttackState::Initialize()
	{
#if 0
		//	アニメーションセット
		owner_->PlayBlendAnimation(Player::AnimationType::ANIM_PUNCH, false, 2.0f);

		//	weight値セット
		//owner_->SetWeight(0.0f);
		owner_->SetWeight(1.0f);

		//	ブレンド率設定
		owner_->SetBlendRate(1.0f);	//	いる?
#endif
		//	アニメーションセット
		//owner_->PlayAnimation(Player::AnimationType::Combo0_1, false, 2.0f, 0.0f);
		owner_->PlayAnimation(Player::AnimationType::Combo0_1, false, 1.0f, 0.0f);

		//	当たり判定タイマー初期化
		judgeTimer_ = 0.0f;
	}

	void AttackState::Update(const float& elapsedTime)
	{
		//	weight加算
		//owner_->AddWeight(elapsedTime);

		//	アニメーション更新処理
		//owner_->UpdateBlendAnimation(elapsedTime);

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
	}

}

//	コンボ01_1(右パンチ)
namespace PlayerState
{
	void ComboOne1::Initialize()
	{
		//	アニメーションセット
		//owner_->PlayAnimation(Player::AnimationType::Combo0_1, false, 1.0f, 0.0f, 0.9f);
		owner_->PlayAnimation(Player::AnimationType::Combo0_1, false, 1.0f, 0.0f, 0.0f);

		//	ルートモーション
		owner_->SetUseRootMotion(true);

		//	判定時間セット
		animJudgeTime_.SetMinJudgeTime(0.55f);
		animJudgeTime_.SetMaxJudgeTime(0.735f);
		inputJudgeTime_.SetMinJudgeTime(0.0f);
		inputJudgeTime_.SetMaxJudgeTime(0.735f);

		//	キー入力判定初期化
		isCorrectInput_ = false;

		//	ステート経過時間初期化
		stateElapsedTime_ = 0.0f;

	}

	void ComboOne1::Update(const float& elapsedTime)
	{
		UpdateElapsedTime(elapsedTime);	//	経過時間更新

		// TODO:アニメーションの長さ調整

		JudgeInput(inputJudgeTime_, GamePad::BTN_B);

		if (IsHit(elapsedTime, animJudgeTime_, "ik_hand_r") == true && isCorrectInput_ == true)
		{
			owner_->ChangeState(Player::StateType::ComboOne2);
			return;
		}
		else if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::Idle);
			return;
		}
	}

	bool ComboOne1::IsHit(const float& elapsedTime, const JudgeTime& animJudgeTime, const std::string& nodeName)
	{
		//	時間での判定
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();
		if (animJudgeTime.IsJudgeFlag(currentAnimationSeconds) == false)
			return false;

		//	ノードと、敵または弾丸との当たり判定当たり判定
		if (owner_->JointVsEnemiesAndBullet(elapsedTime, nodeName, 5.0f) == false)	//	当たっていなかったらコンボキャンセル
		{
			owner_->ChangeState(Player::StateType::Idle);
			return false;
		}

		return true;
	}

	void ComboOne1::JudgeInput(const JudgeTime& inputJudgeTime, const GamePadButton& gamePadButton)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			isCorrectInput_ = true;
			return;
		}

		//	判定時間内に指定したボタンが押されていたらisCorrectInput_をtrueにする
		//if (inputJudgeTime.IsJudgeFlag(stateElapsedTime_) && owner_->GetButtonDown(gamePadButton))
		Command command = { Key6 };
		if (inputJudgeTime_.IsJudgeFlag(stateElapsedTime_) && Input::Instance().CommandConfirm(command, 2.0f))
		{
			isCorrectInput_ = true;
			return;
		}

		//	入力されていなかったらキャンセル
		owner_->ChangeState(Player::StateType::Idle);
		return;
	}

	void ComboOne1::UpdateElapsedTime(const float& elapsedTime)
	{
		stateElapsedTime_ += elapsedTime;
	}

	void ComboOne1::Finalize()
	{
		owner_->SetUseRootMotion(false);
	}

}

//	コンボ01_2(左パンチ)
namespace PlayerState
{
	void ComboOne2::Initialize()
	{
		//	アニメーションセット
		owner_->PlayAnimation(Player::AnimationType::Combo0_2, false, 1.0f, 0.0f);

		//	ルートモーション
		owner_->SetUseRootMotion(true);

		//	判定時間セット
		animJudgeTime_.SetMinJudgeTime(0.230f);
		animJudgeTime_.SetMaxJudgeTime(0.250f);
		inputJudgeTime_.SetMinJudgeTime(0.0f);
		inputJudgeTime_.SetMaxJudgeTime(0.25f);

		//	キー入力判定初期化
		isCorrectInput_ = false;

		//	ステート経過時間初期化
		stateElapsedTime_ = 0.0f;

	}

	void ComboOne2::Update(const float& elapsedTime)
	{
		UpdateElapsedTime(elapsedTime);	//	経過時間更新
		JudgeInput(inputJudgeTime_, GamePad::BTN_B);
		if (IsHit(elapsedTime, animJudgeTime_, "ik_hand_l") == true && isCorrectInput_ == true)	//	ComboOne2も短い
		{
			owner_->ChangeState(Player::StateType::ComboOne3);
			return;
		}
		else if (owner_->IsPlayAnimation() == false)
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
			owner_->ChangeState(Player::StateType::Idle);
			return false;
		}

		return true;
	}

	void ComboOne2::JudgeInput(const JudgeTime& inputJudgeTime, const GamePadButton& gamePadButton)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			isCorrectInput_ = true;
			return;
		}

		//	判定時間内に指定したボタンが押されていたらisCorrectInput_をtrueにする
		//if (inputJudgeTime.IsJudgeFlag(stateElapsedTime_) && owner_->GetButtonDown(gamePadButton))
		Command command = { Key6 };
		if (inputJudgeTime_.IsJudgeFlag(stateElapsedTime_) && Input::Instance().CommandConfirm(command, 3.0f))
		{
			isCorrectInput_ = true;
		}
	}

	void ComboOne2::UpdateElapsedTime(const float& elapsedTime)
	{
		stateElapsedTime_ += elapsedTime;
	}

	void ComboOne2::Finalize()
	{
		owner_->SetUseRootMotion(false);
	}

}

//	コンボ01_3(右アッパー)
namespace PlayerState
{
	void ComboOne3::Initialize()
	{
		//	アニメーションセット
		owner_->PlayAnimation(Player::AnimationType::Combo0_2, false, 1.0f, 0.0f, 0.3f);

		//	ルートモーション
		owner_->SetUseRootMotion(true);

		//	判定時間セット
		animJudgeTime_.SetMinJudgeTime(0.285f);
		animJudgeTime_.SetMaxJudgeTime(0.60f);
		inputJudgeTime_.SetMinJudgeTime(0.0f);
		inputJudgeTime_.SetMaxJudgeTime(0.60f);

		//	キー入力判定初期化
		isCorrectInput_ = false;

		//	ステート経過時間初期化
		stateElapsedTime_ = 0.0f;

	}

	void ComboOne3::Update(const float& elapsedTime)
	{
		UpdateElapsedTime(elapsedTime);	//	経過時間更新
		JudgeInput(inputJudgeTime_, GamePad::BTN_B);
		if (IsHit(elapsedTime, animJudgeTime_, "ik_hand_r") == true && isCorrectInput_ == true)	//	ComboOne3はフレームがめっちゃ短い
		{
			owner_->ChangeState(Player::StateType::ComboOne4);
			return;
		}
		else if (owner_->IsPlayAnimation() == false)
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
			owner_->ChangeState(Player::StateType::Idle);
			return false;
		}

		return true;
	}

	void ComboOne3::JudgeInput(const JudgeTime& inputJudgeTime, const GamePadButton& gamePadButton)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			isCorrectInput_ = true;
			return;
		}

		//	判定時間内に指定したボタンが押されていたらisCorrectInput_をtrueにする
		//if (inputJudgeTime.IsJudgeFlag(stateElapsedTime_) && owner_->GetButtonDown(gamePadButton))
		Command command = { Key6 };
		if (inputJudgeTime_.IsJudgeFlag(stateElapsedTime_) && Input::Instance().CommandConfirm(command, 3.0f))
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
	}

}

//	コンボ01_4(左パンチ)
namespace PlayerState
{
	void ComboOne4::Initialize()
	{
		//	アニメーションセット
		owner_->PlayAnimation(Player::AnimationType::Combo0_3, false, 1.0f, 0.0f,0.085f);

		//	ルートモーション
		owner_->SetUseRootMotion(true);

		//	判定時間セット
		animJudgeTime_.SetMinJudgeTime(0.07f);
		animJudgeTime_.SetMaxJudgeTime(0.11f);
		inputJudgeTime_.SetMinJudgeTime(0.0f);
		inputJudgeTime_.SetMaxJudgeTime(0.11f);

		//	キー入力判定初期化
		isCorrectInput_ = false;

		//	ステート経過時間初期化
		stateElapsedTime_ = 0.0f;

	}

	void ComboOne4::Update(const float& elapsedTime)
	{
		UpdateElapsedTime(elapsedTime);	//	経過時間更新
		JudgeInput(inputJudgeTime_, GamePad::BTN_B);
		if (IsHit(elapsedTime, animJudgeTime_, "ik_hand_l") == true && isCorrectInput_ == true)
		{
			owner_->ChangeState(Player::StateType::ComboOne5);
			return;
		}
		else if (owner_->IsPlayAnimation() == false)
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
			owner_->ChangeState(Player::StateType::Idle);
			return false;
		}

		return true;
	}

	void ComboOne4::JudgeInput(const JudgeTime& inputJudgeTime, const GamePadButton& gamePadButton)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			isCorrectInput_ = true;
			return;
		}

		//	判定時間内に指定したボタンが押されていたらisCorrectInput_をtrueにする
		//if (inputJudgeTime.IsJudgeFlag(stateElapsedTime_) && owner_->GetButtonDown(gamePadButton))
		Command command = { Key6 };
		if (inputJudgeTime_.IsJudgeFlag(stateElapsedTime_) && Input::Instance().CommandConfirm(command, 3.0f))
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
	}
}

//	コンボ01_5(右パンチ)
namespace PlayerState
{
	void ComboOne5::Initialize()
	{
		//	アニメーションセット
		owner_->PlayAnimation(Player::AnimationType::Combo0_3, false, 1.0f, 0.0f, 0.257f);

		//	ルートモーション
		owner_->SetUseRootMotion(true);

		//	判定時間セット
		animJudgeTime_.SetMinJudgeTime(0.262f);
		animJudgeTime_.SetMaxJudgeTime(0.326f);
		inputJudgeTime_.SetMinJudgeTime(0.0f);
		inputJudgeTime_.SetMaxJudgeTime(0.32f);

		//	キー入力判定初期化
		isCorrectInput_ = false;

		//	ステート経過時間初期化
		stateElapsedTime_ = 0.0f;

	}

	void ComboOne5::Update(const float& elapsedTime)
	{
		UpdateElapsedTime(elapsedTime);	//	経過時間更新
		JudgeInput(inputJudgeTime_, GamePad::BTN_B);
		if (IsHit(elapsedTime, animJudgeTime_, "ik_hand_r") == true && isCorrectInput_ == true)
		{
			owner_->ChangeState(Player::StateType::ComboOne6);
			return;
		}
		else if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::Idle);
			return;
		}

	}

	bool ComboOne5::IsHit(const float& elapsedTime, const JudgeTime& animJudgeTime, const std::string& nodeName)
	{
		//	時間での判定
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();
		if (animJudgeTime.IsJudgeFlag(currentAnimationSeconds) == false)
			return false;

		//	ノードと、敵または弾丸との当たり判定当たり判定
		if (owner_->JointVsEnemiesAndBullet(elapsedTime, nodeName, 5.0f) == false)	//	当たっていなかったらコンボキャンセル
		{
			owner_->ChangeState(Player::StateType::Idle);
			return false;
		}

		return true;
	}

	void ComboOne5::JudgeInput(const JudgeTime& inputJudgeTime, const GamePadButton& gamePadButton)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			isCorrectInput_ = true;
			return;
		}

		//	判定時間内に指定したボタンが押されていたらisCorrectInput_をtrueにする
		//if (inputJudgeTime.IsJudgeFlag(stateElapsedTime_) && owner_->GetButtonDown(gamePadButton))
		Command command = { Key6 };
		if (inputJudgeTime_.IsJudgeFlag(stateElapsedTime_) && Input::Instance().CommandConfirm(command, 3.0f))
		{
			isCorrectInput_ = true;
		}
	}

	void ComboOne5::UpdateElapsedTime(const float& elapsedTime)
	{
		stateElapsedTime_ += elapsedTime;
	}

	void ComboOne5::Finalize()
	{
		owner_->SetUseRootMotion(false);
	}

}

//	コンボ01_6(左回し蹴り)
namespace PlayerState
{
	void ComboOne6::Initialize()
	{
		//	アニメーションセット
		owner_->PlayAnimation(Player::AnimationType::Combo0_3, false, 1.0f, 0.0f, 0.424f);

		//	ルートモーション
		owner_->SetUseRootMotion(true);

		//	判定時間セット
		animJudgeTime_.SetMinJudgeTime(0.519f);
		animJudgeTime_.SetMaxJudgeTime(0.931f);
		inputJudgeTime_.SetMinJudgeTime(0.0f);
		inputJudgeTime_.SetMaxJudgeTime(0.93f);

		//	キー入力判定初期化
		isCorrectInput_ = false;

		//	ステート経過時間初期化
		stateElapsedTime_ = 0.0f;

	}

	void ComboOne6::Update(const float& elapsedTime)
	{
		UpdateElapsedTime(elapsedTime);	//	経過時間更新
		JudgeInput(inputJudgeTime_, GamePad::BTN_B);
		if (IsHit(elapsedTime, animJudgeTime_, "ik_foot_l") == true && isCorrectInput_ == true)
		{
			owner_->ChangeState(Player::StateType::ComboOne7);
			return;
		}
		else if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::Idle);
			return;
		}

	}

	bool ComboOne6::IsHit(const float& elapsedTime, const JudgeTime& animJudgeTime, const std::string& nodeName)
	{
		//	時間での判定
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();
		if (animJudgeTime.IsJudgeFlag(currentAnimationSeconds) == false)
			return false;

		//	ノードと、敵または弾丸との当たり判定当たり判定
		if (owner_->JointVsEnemiesAndBullet(elapsedTime, nodeName, 5.0f) == false)	//	当たっていなかったらコンボキャンセル
		{
			owner_->ChangeState(Player::StateType::Idle);
			return false;
		}

		return true;
	}
	
	void ComboOne6::JudgeInput(const JudgeTime& inputJudgeTime, const GamePadButton& gamePadButton)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			isCorrectInput_ = true;
			return;
		}

		//	判定時間内に指定したボタンが押されていたらisCorrectInput_をtrueにする
	//if (inputJudgeTime.IsJudgeFlag(stateElapsedTime_) && owner_->GetButtonDown(gamePadButton))
		Command command = { Key6 };
		if (inputJudgeTime_.IsJudgeFlag(stateElapsedTime_) && Input::Instance().CommandConfirm(command, 3.0f))
		{
			isCorrectInput_ = true;
		}
	}

	void ComboOne6::UpdateElapsedTime(const float& elapsedTime)
	{
		stateElapsedTime_ += elapsedTime;
	}

	void ComboOne6::Finalize()
	{
		owner_->SetUseRootMotion(false);
	}

}

//	コンボ01_7(右打ち下ろし)
namespace PlayerState
{
	void ComboOne7::Initialize()
	{
		//	アニメーションセット
		owner_->PlayAnimation(Player::AnimationType::Combo0_4, false, 1.0f, 0.0f, 0.14f);

		//	ルートモーション
		owner_->SetUseRootMotion(true);

		//	判定時間セット
		animJudgeTime_.SetMinJudgeTime(0.703f);
		animJudgeTime_.SetMaxJudgeTime(1.09f);
		inputJudgeTime_.SetMinJudgeTime(0.0f);
		inputJudgeTime_.SetMaxJudgeTime(1.0f);

		//	キー入力判定初期化
		isCorrectInput_ = false;

		//	ステート経過時間初期化
		stateElapsedTime_ = 0.0f;

	}

	void ComboOne7::Update(const float& elapsedTime)
	{
		UpdateElapsedTime(elapsedTime);	//	経過時間更新
		JudgeInput(inputJudgeTime_, GamePad::BTN_B);
		if (IsHit(elapsedTime, animJudgeTime_, "ik_hand_r") == true)
		{
			
		}
		else if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::Idle);
			return;
		}

	}

	bool ComboOne7::IsHit(const float& elapsedTime, const JudgeTime& animJudgeTime, const std::string& nodeName)
	{
		//	時間での判定
		float currentAnimationSeconds = owner_->GetCurrentAnimationSeconds();
		if (animJudgeTime.IsJudgeFlag(currentAnimationSeconds) == false)
			return false;

		//	ノードと、敵または弾丸との当たり判定当たり判定
		if (owner_->JointVsEnemiesAndBullet(elapsedTime, nodeName, 5.0f) == false)	//	当たっていなかったらコンボキャンセル
		{
			owner_->ChangeState(Player::StateType::Idle);
			return false;
		}

		return true;
	}

	void ComboOne7::JudgeInput(const JudgeTime& inputJudgeTime, const GamePadButton& gamePadButton)
	{
		//	オートコンボがオンなら入力判定をtrueにする
		if (owner_->IsAutoCombo())
		{
			isCorrectInput_ = true;
			return;
		}

		//	判定時間内に指定したボタンが押されていたらisCorrectInput_をtrueにする
		//if (inputJudgeTime.IsJudgeFlag(stateElapsedTime_) && owner_->GetButtonDown(gamePadButton))
		Command command = { Key6 };
		if (inputJudgeTime_.IsJudgeFlag(stateElapsedTime_) && Input::Instance().CommandConfirm(command, 3.0f))
		{
			isCorrectInput_ = true;
		}
	}

	void ComboOne7::UpdateElapsedTime(const float& elapsedTime)
	{
		stateElapsedTime_ += elapsedTime;
	}

	void ComboOne7::Finalize()
	{
		owner_->SetUseRootMotion(false);
	}

}

//	回避ステート
namespace PlayerState
{
	void DodgeState::Initialize()
	{
		owner_->PlayAnimation(Player::AnimationType::DodgeFront, false, 1.0f, 0.0f);
	}

	void DodgeState::Update(const float& elapsedTime)
	{

	}

	void DodgeState::Finalize()
	{

	}
}
