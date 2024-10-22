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
		owner_->PlayAnimation(Player::AnimationType::Idle, true, 1.0f, 0.4f);
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
		if (owner_->GetCombo0ButtonDown())
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
		owner_->MultiplyVelocityXZ(velocityScale_, elapsedTime);
		if (walkTimer_ > walkoToRunInterval_ && owner_->GetCurrentAnimType() != Player::AnimationType::Run)
		{
			owner_->PlayAnimation(Player::AnimationType::Run, true, 1.0f, 0.2f);
		}
		else velocityScale_ += velocityAdd_ * elapsedTime;

		//	移動入力がなくなったら待機ステートへ遷移
		if (!owner_->InputMove(elapsedTime))
		{
			//	待機ステートへ遷移
			owner_->GetStateMachine()->ChangeState(static_cast<int>(Player::StateType::Idle));
			return;
		}
		//	攻撃ステートへ遷移
		//owner_->TransitionAttack();

		//	移動スピード加算
		owner_->AddMoveSpeed(velocityScale_, elapsedTime);

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

	//	拳と敵の当たり判定
	bool AttackState::PunchVsEnemy(const float& elapsedTime, const DirectX::XMFLOAT3& leftHandPos, const float leftHandRadius)
	{
		DirectX::XMFLOAT3 outPosition = {};
		bool isPunchHitEnemy = false;

		for (Enemy* enemy : EnemyManager::Instance().GetEnemies())
		{
			DirectX::XMFLOAT3 ePos = enemy->GetTransform()->GetPosition();
			float eRadius = enemy->GetRadius() + 0.1f;
			float eHeight = enemy->GetHeight() * 2;
			DirectX::XMFLOAT3 ePosOffset = { 0.0f,-eHeight / 2.0f,0.0f };

			//	球と円柱で当たり判定
			//if (1.1f/2.0f < judgeTime_ && judgeTime_ < 1.2f/2.0f)
			//{
			if (Collision::IntersectSphereVsCylinder(leftHandPos, leftHandRadius, ePos + ePosOffset, eRadius, eHeight, outPosition))
			{
				enemy->SubtractHp(1);
				float effectScale = 50.0f;
				isPunchHitEnemy = true;
			}
			else isPunchHitEnemy = false;

			//	エフェクト再生設定(PlayerのRender()で描画される)
			if (isPunchHitEnemy)
			{
				isPunchHitEnemy = true;
				owner_->SetPlayEffectFlag(true);
				owner_->SetEffectPos(leftHandPos);
			}
			//}
		}
		return isPunchHitEnemy;
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

//	コンボ01_1
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
		judgeTime_.SetMinJudgeTime(0.55f);
		judgeTime_.SetMaxJudgeTime(0.735f);

	}

	void ComboOne1::Update(const float& elpasedTime)
	{
		// TODO:アニメーションの長さ調整
		if (owner_->IsPlayAnimation() == false)
		//if (owner_->GetCurrentAnimationSeconds() >= 1.0f)
		{
			owner_->ChangeState(Player::StateType::Idle);
			//owner_->ChangeState(Player::StateType::ComboOne2);
			return;
		}
	}

	void ComboOne1::Finalize()
	{
		owner_->SetUseRootMotion(false);
	}

}

//	コンボ01_2
namespace PlayerState
{
	void ComboOne2::Initialize()
	{
		//	アニメーションセット
		owner_->PlayAnimation(Player::AnimationType::Combo0_2, false, 1.0f, 0.0f);

		//	ルートモーション
		owner_->SetUseRootMotion(true);

		//	判定時間セット


	}

	void ComboOne2::Update(const float& elpasedTime)
	{
		if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::ComboOne3);
			return;
		}
	}

	void ComboOne2::Finalize()
	{
		owner_->SetUseRootMotion(false);
	}

}

//	コンボ01_3
namespace PlayerState
{
	void ComboOne3::Initialize()
	{
		//	アニメーションセット
		owner_->PlayAnimation(Player::AnimationType::Combo0_3, false, 1.0f, 0.0f);

		//	ルートモーション
		owner_->SetUseRootMotion(true);

		//	

	}

	void ComboOne3::Update(const float& elpasedTime)
	{
		if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::ComboOne4);
			return;
		}
	}

	void ComboOne3::Finalize()
	{
		owner_->SetUseRootMotion(false);
	}

}

//	コンボ01_4
namespace PlayerState
{
	void ComboOne4::Initialize()
	{
		//	アニメーションセット
		owner_->PlayAnimation(Player::AnimationType::Combo0_4, false, 1.0f, 0.0f);

		//	ルートモーション
		owner_->SetUseRootMotion(true);

		//	

	}

	void ComboOne4::Update(const float& elpasedTime)
	{
		if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::Idle);
			return;
		}

	}

	void ComboOne4::Finalize()
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
