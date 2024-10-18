#include "Player.h"

#include "../Nova/Graphics/Graphics.h"
#include "../Nova/Input/GamePad.h"
#include "../Nova/Input/Input.h"
#include "../Nova/Graphics/Camera.h"
#include "../Nova/Core/Framework.h"
#include "../Nova/Others/MathHelper.h"
#include "../Nova/Collision/Collision.h"
#include "PlayerState.h"
#include "Stage.h"
#include "EnemyManager.h"

static Player* instance = nullptr;

// インスタンス取得
Player& Player::Instance()
{
	return *instance;
}

//	コンストラクタ
Player::Player()
	:Character("./Resources/Model/free-mixamo-retextured-model/source/model5.glb", "")
{
	//	インスタンス設定
	instance = this;

	//	ステートセット(Player::StateTypeの順と合わせる)
	stateMachine_.reset(new StateMachine<State<Player>>());
	stateMachine_->RegisterState(new PlayerState::IdleState(this));			//	待機
	stateMachine_->RegisterState(new PlayerState::MoveState(this));			//	移動
	stateMachine_->RegisterState(new PlayerState::AttackState(this));		//	攻撃
	stateMachine_->RegisterState(new PlayerState::AvoidanceState(this));	//	回避

	stateMachine_->SetState(static_cast<int>(StateType::Idle));				//	初期ステートセット
	PlayAnimation(Player::AnimationType::ANIM_IDLE, true, 1.0f, 0.0f);

	//	リスナー情報セット
	listener_.innerRadius_ = 0.7f;
	listener_.outerRadius_ = 1.67f;
	listener_.filterParam_ = 0.8f;

}

//	初期化
void Player::Initialize()
{
	//	エフェクト読み込み
	effectResource_ = ResourceManager::Instance().LoadEffectResource("./Resources/Effect/HitEff.efk");

	//	エフェクトスケール設定
	effectScale_ = 5.0f;

	GetTransform()->SetPosition({ 0.0f, 5.0f, 0.0f });

	//float scale = 2.0f;
	GetTransform()->SetScaleFactor(0.0225f);
	radius_ = 0.7f;
	height_ = 4.4f;

	moveSpeed_ = 2.0f;
	//moveSpeed_ = 25.0f;

	hp_ = MAX_HP;

}

//	更新処理
void Player::Update(const float& elapsedTime)
{
	if (!isPose_)	//	ポーズ中じゃないなら
	{
		//	ステートごとの更新処理
		stateMachine_->Update(elapsedTime);

		//	エフェクト再生確認用
#if _DEBUG
		//GamePad& gamePad = Input::Instance().GetGamePad();
		//if (gamePad.GetButtonDown() & GamePad::BTN_A)	//	Zキー入力(エフェクト確認用)
		//{
		//	DirectX::XMFLOAT3 pos = this->GetTransform()->GetPosition();
		//	pos.y += height_;
		//	effectResource_->Play(pos, effectScale_);
		//}
#endif
		//	敵との当たり判定
		PlayerVsEnemy(elapsedTime);

	
		if (!isHitStage_ && isAddGravity_)
		{
			//	適当に重力処理
			AddVelocityY(gravity_, elapsedTime);
			//GetTransform()->SetPositionY(GetTransform()->GetPositionY() - gravity_ * elapsedTime);
			//Move(elapsedTime);	//	inputMoveにもある
		}
		//	ステージとの当たり判定
		if (isCollisionStage_)
		{
			//isHitStage_ = RayVsVertical(elapsedTime);	//	垂直方向(地面)
			if (!RayVsVertical(elapsedTime))
			{
				GetTransform()->AddPositionY(velocity_.y * elapsedTime);
			}
			RayVsHorizontal(elapsedTime);	//	水平方向(壁)
		}

		//Move(elapsedTime);	//	inputMoveにもある

	}

	//	アニメーション更新処理
	UpdateAnimation(elapsedTime);
	
	//DummyRay(elapsedTime);

	//	リスナー更新
	UpdateListener();

}

//	リスナー情報更新
void Player::UpdateListener()
{
	//	処理中に変化する値
	DirectX::XMFLOAT3 position = GetTransform()->GetPosition();
	DirectX::XMFLOAT3 scale = GetTransform()->GetScale();

	//listener_.position = { position.x, position.y + (scale.y / 2.0f), position.z };
	listener_.position_ = { position.x, position.y + height_ / 2, position.z };
	listener_.frontVec_ = Camera::Instance().GetFront();
	listener_.velocity_ = GetMoveVec();
	listener_.rightVec_ = Camera::Instance().GetRight();

}

//	プレイヤーと敵の当たり判定（押し合い処理）
bool Player::PlayerVsEnemy(const float& elapsedTime)
{
	DirectX::XMFLOAT3 pos = GetTransform()->GetPosition();
	pos.y += height_ / 2.0f;
	EnemyManager& enemyManager = EnemyManager::Instance();
	DirectX::XMFLOAT3 outPosition = {};

	isHitEnemy_ = false;

	for (Enemy* enemy : enemyManager.GetEnemies())
	{
		DirectX::XMFLOAT3 ePos = enemy->GetTransform()->GetPosition();
		float eRadius = enemy->GetRadius() - 5.0f;
		float eHeight = enemy->GetHeight();
		DirectX::XMFLOAT3 ePosOffset = { 0.0f,-eHeight / 2.0f,0.0f };

		//	円柱と円柱で当たり判定
		if (Collision::IntersectCylinderVsCyliner(pos, radius_, height_, ePos + ePosOffset, eRadius, eHeight, outPosition, true))
		{
			isHitEnemy_ = true;
			GetTransform()->SetPositionX(outPosition.x);
			GetTransform()->SetPositionZ(outPosition.z);
		}

	}
	return isHitEnemy_;
}

//	移動入力処理
bool Player::InputMove(const float& elapsedTime)
{
	//	進行ベクトル取得
	moveVec_ = GetMoveVec();

	//	移動処理
	UpdateVelocity(elapsedTime);
	Move(elapsedTime);

	//	旋回処理
	Turn(elapsedTime, moveVec_.x, moveVec_.z, turnSpeed_);

	//	進行ベクトルがゼロベクトルでない場合は入力された
	//	(ゼロより大きければ入力された)
	float moveVecLength = sqrtf(moveVec_.x * moveVec_.x + moveVec_.z * moveVec_.z);
	return (moveVecLength > 0);

}

//	攻撃ステートへ遷移
void Player::TransitionAttack()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	//	Xキーを押したら攻撃ステートへ遷移
	if (gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		stateMachine_->ChangeState(static_cast<int>(StateType::Attack));
	}

}

////	ブレンドアニメーション
//void Player::PlayBlendAnimation(AnimationType index, bool loop, float speed)
//{
//	GameObject::PlayBlendAnimation(GetCurrentBlendAnimationIndex(), static_cast<int>(index), loop, speed);
//}

////	ステージとの当たり判定	垂直方向
//bool Player::RayVsVertical(const float& elapsedTime)
//{
//	DirectX::XMFLOAT3 rayStartPos = GetTransform()->GetPosition();	//	レイの始点
//	float centerOffset = height_ / 2.0f;							//	始点をプレイヤーの中心へ補正
//	rayStartPos.y += centerOffset;
//	DirectX::XMFLOAT3	rayDirection = { 0,-1,0 };					//	レイの方向(真下)
//	DirectX::XMFLOAT4X4 transform	= {};							//	ステージのワールド変換行列
//	DirectX::XMStoreFloat4x4(&transform, Stage::Instance().GetTransform()->CalcWorld());
//
//	float	rayLimit = centerOffset - velocity_.y * elapsedTime;	//	レイの長さ
//	//float	rayLimit = 9.0f;	//	レイの長さ
//	bool	skipIf = false;
//
//	//	レイの開始点描画
//#if 1
//	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
//	debugRenderer->DrawSphere(rayStartPos, rayPosRadius_, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));	//	青
//#endif
//
//	//	レイの終点描画
//#if 1
//	DirectX::XMFLOAT3 rayEndPos = {};
//	DirectX::XMVECTOR RayEndPos = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&rayStartPos), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&rayDirection), rayLimit));
//	DirectX::XMStoreFloat3(&rayEndPos, RayEndPos);
//	debugRenderer = Graphics::Instance().GetDebugRenderer();
//	debugRenderer->DrawSphere(rayEndPos, rayPosRadius_, DirectX::XMFLOAT4(0.0f, 1.0f, 0.6f, 1.0f));		//	緑
//#endif
//
//	DirectX::XMFLOAT3	intersectionPosition	= {};			//	当たった位置
//	DirectX::XMFLOAT3	intersectionNormal		= {};			//	法線の方向
//	std::string			intersectionMesh		= {};			//	メッシュ名
//	std::string			intersectionMaterial	= {};			//	マテリアル名
//
//	//	当たり判定処理
//	bool isHit = false;
//	isHit = Stage::Instance().Collision(rayStartPos, rayDirection, transform, intersectionPosition, intersectionNormal, intersectionMesh, intersectionMaterial, rayLimit, skipIf);
//
//	//	地面にレイが当たったら
//	if (isHit)
//	{
//		hitPosition_ = intersectionPosition;
//		hitNormal_ = intersectionNormal;
//#if 0 
//		DirectX::XMFLOAT3 pos = {};
//		DirectX::XMVECTOR intersectPos = DirectX::XMLoadFloat3(&intersectionPosition);
//		DirectX::XMVECTOR pushVec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&intersectionNormal)), rayLimit);
//		
//		DirectX::XMStoreFloat3(&pos, DirectX::XMVectorSubtract(intersectPos, pushVec));
//
//		GetTransform()->SetPosition(pos);
//#else
//		DirectX::XMVECTOR	IntersectionPos = DirectX::XMLoadFloat3(&intersectionPosition);			//	レイが当たった位置
//		DirectX::XMFLOAT3	pos				= GetTransform()->GetPosition();						//	プレイヤーの現在の位置(足元)
//		DirectX::XMVECTOR	Pos				= DirectX::XMLoadFloat3(&pos);							//	プレイヤーの現在の位置ベクトル
//		DirectX::XMVECTOR	Push			= DirectX::XMVectorSubtract(IntersectionPos, Pos);		//	当たった位置からプレイヤーまでのベクトル
//		
//		DirectX::XMVectorSetX(Push, 0.0f);
//		DirectX::XMVectorSetZ(Push, 0.0f);
//
//		float				pushLength		= DirectX::XMVectorGetY(DirectX::XMVector3Length(Push));
//		DirectX::XMFLOAT3	push = {};
//		DirectX::XMStoreFloat3(&push, Push);
//		if (pushLength > 0.0001f)
//		{
//			//GetTransform()->SetPosition(intersectionPosition);
//			//GetTransform()->AddPosition(push);
//			//GetTransform()->AddPositionY(push.y);
//		}
//
//		DirectX::XMFLOAT3 posDebug = GetTransform()->GetPosition();
//
//		/*DirectX::XMVECTOR Velocity = DirectX::XMLoadFloat3(&velocity_);
//		Velocity = DirectX::XMVectorAdd(Velocity, Push);
//		DirectX::XMStoreFloat3(&velocity_, Velocity);*/
//
//		velocity_.y = 0.0f;
//#endif
//
//		//	レイが当たった位置
//#if 1
//		DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
//		debugRenderer->DrawSphere(intersectionPosition, rayPosRadius_ + 1.0f, DirectX::XMFLOAT4(1, 1, 1, 1));	//	白
//#endif
//		
//	}
//
//	return isHit;
//}

//	COLLISION_MESH_2
//	ステージとの当たり判定	垂直方向
bool Player::RayVsVertical(const float& elapsedTime)
{
	DirectX::XMFLOAT3 rayStartPos;									//	レイの始点
	DirectX::XMFLOAT3 rayDirection;									//	レイの方向
	float liftup = height_ / 2.0f;									//	レイの始点をプレイヤーの中心へ持ち上げる
	DirectX::XMVECTOR RayPos = DirectX::XMLoadFloat3(&GetTransform()->GetPosition());							//	レイの始点
	DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));	//	レイの方向
	DirectX::XMVECTOR Liftup = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.0f, liftup, 0.0f, 1.0f));		//	LIFTUP
	DirectX::XMStoreFloat3(&rayStartPos, DirectX::XMVectorAdd(RayPos, Liftup));
	DirectX::XMStoreFloat3(&rayDirection, Direction);

	DirectX::XMFLOAT3 playerPos = GetTransform()->GetPosition();	//	プレイヤーの位置(足元が基準点)

	DirectX::XMFLOAT4X4 transform = {};								//	ステージのワールド変換行列
	DirectX::XMStoreFloat4x4(&transform, Stage::Instance().GetTransform()->CalcWorld());

	//	レイの開始点描画
#if 1
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
	debugRenderer->DrawSphere(rayStartPos, rayPosRadius_, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));	//	青
#endif

	//	Collision()の結果格納用
	DirectX::XMFLOAT3	intersectionPosition = {};			//	当たった位置
	DirectX::XMFLOAT3	intersectionNormal = {};			//	法線の方向
	std::string			intersectionMesh = {};				//	メッシュ名
	std::string			intersectionMaterial = {};			//	マテリアル名

	//	当たり判定処理
	bool isHit = false;
	//	レイと地面が当たっていたら
	if (Stage::Instance().Collision(rayStartPos, rayDirection, transform, intersectionPosition, intersectionNormal, intersectionMesh, intersectionMaterial))
	{
		float d0 = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&playerPos) - DirectX::XMLoadFloat3(&rayStartPos)));
		float d1 = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&intersectionPosition) - DirectX::XMLoadFloat3(&rayStartPos)));

		//	プレイヤーと地面が当たっていたら
		if (d0 + radius_ > d1)
		{
			//	プレイヤーの位置を補正
			float d = d0 - d1;
			playerPos.x -= d * rayDirection.x;
			playerPos.y -= d * rayDirection.y;
			playerPos.z -= d * rayDirection.z;

			GetTransform()->SetPosition(playerPos);

			// Reflection
			DirectX::XMStoreFloat3(&velocity_, DirectX::XMVector3Reflect(DirectX::XMLoadFloat3(&velocity_), DirectX::XMLoadFloat3(&intersectionNormal)));

			isHit = true;

			//	デバッグ描画
			//	レイが当たった位置
#if 1
			DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
			debugRenderer->DrawSphere(intersectionPosition, rayPosRadius_ + 1.0f, DirectX::XMFLOAT4(1, 1, 1, 1));	//	白
#endif

		}

	}

	return isHit;
}

//	ステージとの当たり判定	水平方向
//bool Player::RayVsHorizontal(const float& elapsedTime)
//{
//	//	水平速力計算
//	float velocityLengthXZ = sqrtf(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
//	if (velocityLengthXZ > 0.0f)
//	{
//		//	水平移動値
//		float mx = velocity_.x * elapsedTime;
//		float mz = velocity_.z * elapsedTime;
//
//		//	レイの開始位置と方向
//		DirectX::XMFLOAT3	position		= GetTransform()->GetPosition();
//		DirectX::XMFLOAT3	rayStartPos			= position;
//		DirectX::XMFLOAT3	radiusOffset	= {};
//		DirectX::XMVECTOR	Move			= DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&GetMoveVec()));
//		Move = DirectX::XMVectorSetY(Move, 0.0f);
//		DirectX::XMVECTOR	RadiusOffset	= DirectX::XMVectorScale(Move, radius_);				//	移動方向の向きに半径を足す
//		DirectX::XMStoreFloat3(&radiusOffset, RadiusOffset);
//		float				heightOffset	= height_ / 2.0f;										//	レイの始点をプレイヤーの中心へ補正
//		rayStartPos.y += heightOffset;
//
//		DirectX::XMFLOAT3	rayEnd			= { rayStartPos.x + mx  ,rayStartPos.y , rayStartPos.z + mz };	//	レイの終了地点
//		DirectX::XMVECTOR	RayPos			= DirectX::XMLoadFloat3(&rayStartPos);				//	レイの開始点
//		DirectX::XMVECTOR	RayEnd			= DirectX::XMLoadFloat3(&rayEnd);
//		RayEnd = DirectX::XMVectorAdd(RayEnd, Move);
//		float				rayLimit		= DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(RayEnd, RayPos))) + radius_;
//		DirectX::XMFLOAT3	rayDirection	= {};
//		DirectX::XMVECTOR	RayDirection	= DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(RayEnd, RayPos));
//		DirectX::XMStoreFloat3(&rayDirection, RayDirection);
//		//	水平に飛ばす
//		rayDirection.y = 0;
//
//		//	ステージのワールド変換行列
//		DirectX::XMFLOAT4X4 transform = {};							
//		DirectX::XMStoreFloat4x4(&transform, Stage::Instance().GetTransform()->CalcWorld());
//
//		//	レイの開始点描画
//#if 1
//		DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
//		debugRenderer->DrawSphere(rayStartPos, rayPosRadius_, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));	//	青
//#endif
//
//		//	レイの終点描画
//#if 0
//		DirectX::XMFLOAT3 rayEndPos = {};
//		DirectX::XMVECTOR RayEndPos = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&rayStartPos), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&rayDirection), rayLimit));
//		DirectX::XMStoreFloat3(&rayEndPos, RayEndPos);
//		debugRenderer = Graphics::Instance().GetDebugRenderer();
//		debugRenderer->DrawSphere(rayEndPos, rayPosRadius_, DirectX::XMFLOAT4(0.0f, 1.0f, 0.6f, 1.0f));	//	緑
//#endif
//
//		//	レイキャストによる壁判定
//		DirectX::XMFLOAT3	intersectionPosition	= {};			//	当たった位置
//		DirectX::XMFLOAT3	intersectionNormal		= {};			//	法線の方向
//		std::string			intersectionMesh		= {};			//	メッシュ名
//		std::string			intersectionMaterial	= {};			//	マテリアル名
//		bool skipIf = false;
//
//		if (Stage::Instance().Collision(rayStartPos, rayDirection, transform, intersectionPosition, intersectionNormal, intersectionMesh, intersectionMaterial, rayLimit, skipIf))
//		{
//			//	壁までのベクトル
//			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&intersectionPosition);
//			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&rayEnd);
//			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
//
//			//	壁の法線
//			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&intersectionNormal);
//
//			//	入射ベクトルを法線に射影
//			float projectionL = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal)) * 1.5f;
//
//			//	補正位置の計算
//			DirectX::XMVECTOR CollectPosition = DirectX::XMVectorAdd(
//				DirectX::XMVectorScale(Normal, projectionL), End);
//			DirectX::XMFLOAT3 collectPosition{};
//			DirectX::XMStoreFloat3(&collectPosition, CollectPosition);
//
//			//	壁ずり後の位置がめりこんでいないかチェック
//			intersectionPosition	= {};			//	当たった位置
//			intersectionNormal		= {};			//	法線の方向
//			intersectionMesh		= {};			//	メッシュ名
//			intersectionMaterial	= {};			//	マテリアル名
//			DirectX::XMStoreFloat3(&rayEnd, CollectPosition);
//			rayLimit = DirectX::XMVectorGetX(DirectX::XMVectorSubtract(RayEnd, RayPos));
//			rayDirection = {};
//			RayDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(RayEnd, RayPos));
//			DirectX::XMStoreFloat3(&rayDirection, RayDirection);
//
//			if (Stage::Instance().Collision(rayStartPos, rayDirection, transform, intersectionPosition, intersectionNormal, intersectionMesh, intersectionMaterial, rayLimit, skipIf))
//			{
//				CollectPosition = DirectX::XMLoadFloat3(&intersectionPosition);
//			}
//			else
//			{
//				position.x = collectPosition.x;
//				position.z = collectPosition.z;
//			}
//
//			//	レイが当たった位置
//#if 1
//			DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
//			debugRenderer->DrawSphere(intersectionPosition, rayPosRadius_, DirectX::XMFLOAT4(1, 1, 1, 1));	//	白
//#endif
//
//		}
//		else
//		{
//			////	移動
//			//position.x += mx;
//			//position.z += mz;
//		}
//
//		GetTransform()->SetPosition(position);
//	}
//
//	return false;
//}

bool Player::RayVsHorizontal(const float& elapsedTime)
{
	DirectX::XMFLOAT3 rayStartPos;									//	レイの始点
	DirectX::XMFLOAT3 rayDirection;									//	レイの方向
	float liftup = height_ / 2.0f;									//	レイの始点をプレイヤーの中心へ持ち上げる
	DirectX::XMVECTOR RayPos = DirectX::XMLoadFloat3(&GetTransform()->GetPosition());							//	レイの始点
	DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(DirectX::XMVectorSet(velocity_.x, 0.0f, velocity_.z, 0.0f));	//	レイの方向
	DirectX::XMVECTOR Liftup = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.0f, liftup, 0.0f, 1.0f));		//	LIFTUP
#if 1
	DirectX::XMStoreFloat3(&rayStartPos, DirectX::XMVectorAdd(RayPos, Liftup));
#else
	float stepBack = 1.0f;
	DirectX::XMStoreFloat3(&rayStartPos, DirectX::XMVectorSubtract(RayPos, DirectX::XMVectorScale(Direction, stepBack)));
#endif
	DirectX::XMStoreFloat3(&rayDirection, Direction);

	DirectX::XMFLOAT3 playerPos = GetTransform()->GetPosition();	//	プレイヤーの位置(足元が基準点)

	DirectX::XMFLOAT4X4 transform = {};								//	ステージのワールド変換行列
	DirectX::XMStoreFloat4x4(&transform, Stage::Instance().GetTransform()->CalcWorld());

	//	レイの開始点描画
#if 1
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
	debugRenderer->DrawSphere(rayStartPos, rayPosRadius_, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));	//	青
#endif

	//	Collision()の結果格納用
	DirectX::XMFLOAT3	intersectionPosition = {};			//	当たった位置
	DirectX::XMFLOAT3	intersectionNormal = {};			//	法線の方向
	std::string			intersectionMesh = {};				//	メッシュ名
	std::string			intersectionMaterial = {};			//	マテリアル名

	//	当たり判定処理
	bool isHit = false;
	//	レイと地面が当たっていたら
	if (Stage::Instance().Collision(rayStartPos, rayDirection, transform, intersectionPosition, intersectionNormal, intersectionMesh, intersectionMaterial))
	{
		float d0 = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&playerPos) - DirectX::XMLoadFloat3(&rayStartPos)));
		float d1 = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&intersectionPosition) - DirectX::XMLoadFloat3(&rayStartPos)));

		float rayOffset = 0.5f;	//	レイの長さを少し増やす

		//	プレイヤーと地面が当たっていたら
		if (d0 + radius_ + rayOffset > d1)
		{
			//	プレイヤーの位置を補正
			float d = d0 - d1;
			playerPos.x -= d * rayDirection.x;
			playerPos.y -= d * rayDirection.y;
			playerPos.z -= d * rayDirection.z;

			GetTransform()->SetPosition(playerPos);

			// Reflection
			DirectX::XMStoreFloat3(&velocity_, DirectX::XMVector3Reflect(DirectX::XMLoadFloat3(&velocity_), DirectX::XMLoadFloat3(&intersectionNormal)));

			isHit = true;

			//	デバッグ描画
			//	レイが当たった位置
#if 1
			DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
			debugRenderer->DrawSphere(intersectionPosition, rayPosRadius_ + 1.0f, DirectX::XMFLOAT4(1, 1, 1, 1));	//	白
#endif

		}

	}

	return isHit;
}

//	エフェクト再生
void Player::PlayEffect()
{
	//	エフェクト描画
	effectResource_->Play(effectPos_, effectScale_);

	//	エフェクト描画フラグリセット
	playEffectFlag_ = false;
}

bool Player::DummyRay(const float& elapsedTime)
{
	//	右手のワールド座標取得
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, GetTransform()->CalcWorld());	//	プレイヤーのワールド行列
	DirectX::XMFLOAT3 leftHandPos = GetJointPosition("Ch44", "mixamorig:RightHandMiddle1", world);

	//	当たり判定用の半径セット
	constexpr float leftHandRadius = 50.0f;

	//	衝突判定用のデバッグ球を描画
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
	debugRenderer->DrawSphere(leftHandPos, leftHandRadius, DirectX::XMFLOAT4(1, 1, 1, 1));

	DirectX::XMFLOAT3	rayPos = leftHandPos;					//	レイの始点
	DirectX::XMFLOAT3	rayDirection = { 0,-1,0 };              //	レイの方向(真下)
	DirectX::XMFLOAT4X4 transform = {};							//	ステージのワールド変換行列
	DirectX::XMStoreFloat4x4(&transform, Stage::Instance().GetTransform()->CalcWorld());
	float	rayLimit = dummyRayLimit_;							//	レイの長さ(そのときのvelocity)
	bool	skipIf = true;										//	

	DirectX::XMFLOAT3 rayEndPos = {};
	DirectX::XMVECTOR RayEndPos = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&rayPos), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&rayDirection), rayLimit));
	DirectX::XMStoreFloat3(&rayEndPos, RayEndPos);
	isDummyHit_ = Stage::Instance().Collision(rayPos, rayDirection, transform, hitPosition_, hitNormal_, hitMaterial_, hitMesh_, rayLimit, skipIf);
	
	//	レイが当たっていなかったらヒット情報リセット
	if (isDummyReset_)
	{
		hitPosition_	= {};
		hitNormal_		= {};
		hitMesh_		= {};
		hitMaterial_	= {};
	}

	return isDummyHit_;
}

//	アニメーション
void Player::PlayAnimation(AnimationType index, const bool& loop, const float& speed, const float blendTime, const float cutTime)
{
	Character::PlayAnimation(static_cast<int>(index), loop, speed, blendTime, cutTime);
}

//	スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec()const
{
	//	入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//	カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFoward = camera.GetFront();

	//	移動ベクトルはXZ平面に水平なベクトルになるようにする
	//	カメラ右方向ベクトルをXZ単位ベクトルに変換
	float Rlength;
	DirectX::XMStoreFloat(&Rlength, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&cameraRight)));
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		//	単位ベクトル化
		cameraRightX = cameraRight.x / Rlength;
		cameraRightZ = cameraRight.z / Rlength;
	}

	//	カメラ前方向ベクトルをXZ単位ベクトルに変換
	float Zlength;
	DirectX::XMStoreFloat(&Zlength, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&cameraFoward)));
	float cameraFrontX = cameraFoward.x;
	float cameraFrontZ = cameraFoward.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		//	単位ベクトル化
		cameraFrontX = cameraFoward.x / Zlength;
		cameraFrontZ = cameraFoward.z / Zlength;
	}

	//	スティックの水平入力値をカメラ右方向に反映し、
	//	スティック垂直入力値をカメラ前方向に反映し、
	//	進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraFrontX * ay + cameraRightX * ax) * moveSpeed_;
	vec.z = (cameraFrontZ * ay + cameraRightZ * ax) * moveSpeed_;

	//	Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;
}

//	描画処理
void Player::Render()
{
	//	ピクセルシェーダーセット
	//Graphics::Instance().GetShader()->CreatePsFromCso(Graphics::Instance().GetDevice(), "./Resources/Shader/DronePS.cso", pixelShader_.ReleaseAndGetAddressOf());
	//this->SetPixelShader(pixelShader_.Get());
	Character::Render();

	//	エフェクト描画
	Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_OFF_ZW_OFF);
	if (playEffectFlag_)PlayEffect();

}

//	現在再生中のアニメーション番号取得
int Player::GetCurrentAnimNum()
{
	return Character::GetCurrentAnimNum();
}

//	現在再生中のアニメーションタイプ取得
Player::AnimationType Player::GetCurrentAnimType()
{
	int currentAnimNum = Character::GetCurrentAnimNum();

	return static_cast<Player::AnimationType>(currentAnimNum);
}

//	デバッグ描画
void Player::DrawDebug()
{
	//	ステート文字列
	std::string stateStr[static_cast<int>(StateType::Max)] =
	{
		"Idle","Move","Attack","Avoidance"
	};

	//	ステージヒット文字列
	std::string hitStage = "";
	if (isHitStage_)hitStage = "true";
	else hitStage = "false";

	//	アニメーション関連
	//int currentAnimationIndex = GetCurrentBlendAnimationIndex();	//	現在のアニメーション番号取得
	//float weight = GetWeight();										//	weight値取得		
	//float blendRate = GetBlendRate();

	if (ImGui::TreeNode(u8"Playerプレイヤー"))
	{
		ImGui::Checkbox("IsPose", &isPose_);
		ImGui::Checkbox("PlayEffect", &playEffectFlag_);
		ImGui::Checkbox("Invincible", &isInvincible_);
		ImGui::Checkbox("AddGravity", &isAddGravity_);

		Character::DrawDebug();
	
		ImGui::DragFloat("Gravity", &gravity_, 0.1f, 0.0f);												//	重力
		ImGui::DragFloat("EffectScale", &effectScale_, 0.01f, -FLT_MAX, FLT_MAX);						//	エフェクトスケール
		//ImGui::DragFloat("AnimationSpeed", &animationSpeed_, 0.01f, -FLT_MAX, FLT_MAX);				//	アニメーション再生速度
		//ImGui::DragFloat("AnimationWeight", &weight, 0.005f, 0.0f, 1.0f);								//	アニメーションweight値
		//ImGui::DragFloat("BlendRate", &blendRate, 0.005f, 0.0f, 1.0f);								//	アニメーションブレンド率
		//ImGui::InputInt("CurrentBlendAnimationIndex", &currentAnimationIndex);						//	現在のアニメーション番号
		ImGui::Text(u8"State　%s", stateStr[static_cast<int>(stateMachine_->GetStateIndex())].c_str());	//	ステート表示
		ImGui::Checkbox(u8"StageCollision", &isCollisionStage_);										//	ステージとの当たり判定オン/オフ
		ImGui::Text(u8"HitStage %s", hitStage.c_str());													//	ステージと当たっているか
		ImGui::DragFloat("Gravity", &gravity_, 0.01f, -FLT_MAX, FLT_MAX);								//	重力
		ImGui::DragFloat("MoveSpeed", &moveSpeed_, 0.01f, 0.0f, FLT_MAX);								//	移動する速さ
		
		ImGui::DragFloat("RayPosRadius", &rayPosRadius_);	//	レイキャストの始点終点を表す球の半径
		
		ImGui::DragFloat3("ConeDirection", &coneDirection_.x, 0.01f, -FLT_MAX, FLT_MAX);
		
		DrawDummyRay();

		if (ImGui::TreeNode("3DAudio_Listener"))
		{
			//	リスナー情報
			ImGui::DragFloat3("Position", &listener_.position_.x);
			ImGui::DragFloat("InnerRadius", &listener_.innerRadius_);
			ImGui::DragFloat("OuterRadius", &listener_.outerRadius_);
			ImGui::DragFloat("FilterParam", &listener_.filterParam_);
			ImGui::DragFloat3("FrontVec", &listener_.frontVec_.x);
			ImGui::DragFloat3("RightVec", &listener_.rightVec_.x);
			ImGui::DragFloat3("Velocity", &listener_.velocity_.x);

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	//	ImGuiでの変化を反映させる
	//SetWeight(weight);
	//SetBlendRate(blendRate);

}

//	デバッグプリミティブ描画
void Player::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//	衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawCylinder(this->GetTransform()->GetPosition(), radius_, height_, DirectX::XMFLOAT4(0, 0, 0, 1));

	//	円錐を描画
	debugRenderer->DrawCone(this->GetTransform()->GetPosition(), coneDirection_, radius_, height_, DirectX::XMFLOAT4{ 0,0,0,1 });

}

void Player::DrawDummyRay()
{
	if (ImGui::TreeNode(u8"RayHIt"))
	{
		//ImGui::Checkbox("DummyHitReset", &isDummyReset_);
		//ImGui::Checkbox("DummyHit", &isDummyHit_);
		//ImGui::DragFloat("RayLimit", &dummyRayLimit_, 0.1f, -FLT_MAX, FLT_MAX);
		//ImGui::DragFloat("RayDebugOffset", &debugOffset_, 0.1f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat3("HitPos", &hitPosition_.x, 0.01f, -FLT_MAX, FLT_MAX);				//	当たった位置
		ImGui::DragFloat3("HitNormal", &hitNormal_.x, 0.01f, -FLT_MAX, FLT_MAX);			//	当たった面の法線
		ImGui::Text("Mesh", &hitMesh_);														//	メッシュ名
		ImGui::Text("Material", &hitMaterial_);												//	マテリアル名

		ImGui::TreePop();
	}

}