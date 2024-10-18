#include "PlayerState.h"

#include "../Nova/Debug/DebugRenderer.h"
#include "../Nova/Graphics/Graphics.h"
#include "../Nova/Collision/Collision.h"
#include "../Nova/Others/MathHelper.h"
#include "BulletManager.h"
#include "EnemyManager.h"

#include <algorithm>

//	待機ステート
namespace PlayerState
{
	void IdleState::Initialize()
	{
#if 0
		//	アニメーションセット
		owner_->SetWeight(0.0f);
		if (owner_->GetCurrentBlendAnimationIndex() == -1)
		{
			owner_->PlayBlendAnimation(Player::AnimationType::ANIM_T, Player::AnimationType::ANIM_IDLE, true);
			owner_->SetWeight(1.0f);
		}
		else if (owner_->GetCurrentBlendAnimationIndex() == static_cast<int>(Player::AnimationType::ANIM_WALK))
		{
			owner_->PlayBlendAnimation(Player::AnimationType::ANIM_IDLE, true);
			owner_->SetWeight(0.7f);
		}
		else
		{
			owner_->PlayBlendAnimation(Player::AnimationType::ANIM_IDLE, true);
		}
#endif
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
		}
		////	アニメーション更新処理
		//owner_->UpdateBlendAnimation(elapsedTime);

#if 0
		owner_->AddWeight(elapsedTime);
		owner_->SetWeight(std::clamp(owner_->GetWeight(), 0.0f, 1.0f));
#endif
		//	攻撃ステートへ遷移
		owner_->TransitionAttack();

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
		walkTimer_ += WALK_TIMER_ADD * elapsedTime;
		owner_->MultiplyVelocityXZ(velocityScale_, elapsedTime);
		if (walkTimer_ > WALK_TO_RUN_INTERVAL && owner_->GetCurrentAnimType() != Player::AnimationType::Run)
		{
			owner_->PlayAnimation(Player::AnimationType::Run, true, 1.0f, 0.5f);
		}
		else velocityScale_ += 0.1f * elapsedTime;

		//	移動入力がなくなったら待機ステートへ遷移
		if (!owner_->InputMove(elapsedTime))
		{
			//	待機ステートへ遷移
			owner_->GetStateMachine()->ChangeState(static_cast<int>(Player::StateType::Idle));

		}
		//	攻撃ステートへ遷移
		owner_->TransitionAttack();

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

	}

	void AttackState::Update(const float& elapsedTime)
	{
		//	weight加算
		//owner_->AddWeight(elapsedTime);

		//	アニメーション更新処理
		//owner_->UpdateBlendAnimation(elapsedTime);

#if 1
		//	アニメーション再生が終わったら待機ステートへ遷移
		if (!owner_->IsPlayAnimation()/* && isMove_ == false*/)
		{
			owner_->GetStateMachine()->ChangeState(static_cast<int> (Player::StateType::Idle));
		}
#endif
		//	攻撃処理
		PunchAttack(elapsedTime);



	}

	void AttackState::Attack()
	{

	}

	//	殴打攻撃（普通の攻撃）
	void AttackState::PunchAttack(const float& elapsedTime)
	{
		//	右手のワールド座標取得
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, owner_->GetTransform()->CalcWorld());	//	プレイヤーのワールド行列
		//DirectX::XMStoreFloat4x4(&world, owner_->GetTransform()->CalcWorldMatrix(scale));	//	プレイヤーのワールド行列
		DirectX::XMFLOAT3 leftHandPos = owner_->GetJointPosition("SKM_Manny_LOD0", "ik_hand_gun", world);

		//	当たり判定用の半径セット
		float leftHandRadius = 3.0f;

		//	衝突判定用のデバッグ球を描画
		DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
		//debugRenderer->DrawSphere(leftHandPos, leftHandRadius, DirectX::XMFLOAT4(1, 1, 1, 1));

		//	弾丸への攻撃判定
		PuchVsBullet(elapsedTime, leftHandPos, leftHandRadius);

		//	敵への当たり判定
		PunchVsEnemy(elapsedTime, leftHandPos, leftHandRadius);

	}

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
			judgeTime_ += elapsedTime;
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
		judgeTime_ = 0.0f;
	}

}

//	回避ステート
namespace PlayerState
{
	void AvoidanceState::Initialize()
	{
		owner_->PlayAnimation(Player::AnimationType::DodgeFront, false, 1.0f, 0.0f);
	}

	void AvoidanceState::Update(const float& elapsedTime)
	{

	}

	void AvoidanceState::Finalize()
	{

	}
}
