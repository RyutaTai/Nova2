#pragma once

#include <memory>

#include "Character.h"
#include "../Nova/Resources/Effect.h"
#include "../Nova/AI/StateMachine.h"
#include "../Nova/Audio/Audio3DSystem.h"

class Player :public Character
{
public:
	//	アニメーション情報
	enum class AnimationType
	{
		ANIM_T = 0,		//	Tポーズ
		ANIM_IDLE,		//	待機
		ANIM_WALK,		//	歩き
		ANIM_RUN,		//	走り
		ANIM_PUNCH,		//	パンチ
		ANIM_PUNCH2,	//	パンチ2
		ANIM_WINCE,		//	ひるみ	
		ANIM_DOWN,		//	ダウン
		ANIM_AVOID,		//	回避
		Max,			//	アニメーション最大数
	};

	//	ステートの種類
	enum class StateType
	{
		Idle = 0, 		//	待機
		Move,			//	移動
		Attack,			//	攻撃
		Avoidance,		//	回避
		Max,			//	ステート最大数
	};

public:
	Player();
	~Player()override {}

	static Player& Instance();

	void Initialize()override;
	void Update(const float& elapsedTime)override;
	void Render()override;
	
	//	デバッグ
	void DrawDebug()	override;
	void DrawDebugPrimitive();
	void DrawDummyRay();

	bool InputMove(const float& elapsedTime);		//	移動入力処理
	void TransitionAttack();						//	攻撃ステートへ遷移
	void ChangeState(StateType state) { stateMachine_->ChangeState(static_cast<int>(state)); }	//	ステート遷移
	void PlayEffect();

	//	判定
	bool RayVsVertical(const float& elapsedTime)override;		//	ステージとの当たり判定(垂直方向)
	bool RayVsHorizontal(const float& elapsedTime)override;		//	ステージとの当たり判定(水平方向)	
	bool DummyRay(const float& elapsedTime);
	bool PlayerVsEnemy(const float& elapsedTime);

	void PlayAnimation(AnimationType index, const bool& loop = false, const float& speed = 1.0f, const float blendTime = 1.0f, const float cutTime = 0.0f);
	//void PlayBlendAnimation(AnimationType index, bool loop, float speed = 1.0f);
	//void PlayBlendAnimation(AnimationType index1, AnimationType index2, bool loop, float speed = 1.0f) { GameObject::PlayBlendAnimation(static_cast<int>(index1), static_cast<int>(index2), loop, speed); }
	
	void UpdateListener();	//	リスナー情報更新

	void SetIsPose(bool isPose)							{ isPose_ = isPose; }
	void SetIsHitEnemy(bool isHitEnemy)					{ isHitEnemy_ = isHitEnemy; }
	void SetEffectScale(float scale)					{ effectScale_ = scale; }
	void SetPlayEffectFlag(bool playEffect)				{ playEffectFlag_ = playEffect; }
	void SetEffectPos(const DirectX::XMFLOAT3& pos)		{ effectPos_ = pos; }

	bool GetPose()										{ return isPose_; }
	bool IsHItEnemy()									{ return isHitEnemy_; }
	bool IsPlayEffect()									{ return playEffectFlag_; }
	int								GetCurrentAnimNum();	//	現在再生中のアニメーション番号取得
	AnimationType					GetCurrentAnimType();	//	現在再生中のアニメーションタイプ取得
	DirectX::XMFLOAT3				GetMoveVec()const;		//スティック入力値から移動ベクトルを取得
	StateMachine<State<Player>>*	GetStateMachine()	{ return stateMachine_.get(); }		//	ステートマシン取得
	SoundListener					GetListener()const	{ return listener_; }				//	リスナー取得

private:
	std::shared_ptr <Effect>		effectResource_;										//	エフェクト
	float							effectScale_ = 5.0f;									//	エフェクトスケール
	DirectX::XMFLOAT3				effectPos_ = {};										//	エフェクト再生位置
	bool							playEffectFlag_ = false;								//	エフェクト描画フラグ
	//AnimationType					currentAnimNum_;										//	現在のアニメーション番号
	float							turnSpeed_ = DirectX::XMConvertToRadians(720);			//	旋回速度
	std::unique_ptr<StateMachine<State<Player>>>	stateMachine_ = nullptr;				//	ステートマシン
	bool							isPose_ = false;										//	ポーズ中プレイヤーの操作を受け付けない
	bool							isHitEnemy_ = false;									//	エネミーと当たっているか(押し出し用)

	//	オーディオ
	SoundListener listener_ = {};	//	リスナー

	//	ImGui用
	bool				isCollisionStage_	= true;
	bool				isHitStage_			= false;
	float				gravity_			= -5.0f;
	float				rayPosRadius_		= 0.3f;
	DirectX::XMFLOAT3	coneDirection_		= { 0,1,0 };	//	円錐の方向
	bool				isAddGravity_		= false;		//	重力加算フラグ

	//	DummyRay
	float debugOffset_ = height_ / 2;
	bool				isDummyReset_		= false;
	bool				isDummyHit_			= false;		//	当てっているかどうか
	float				dummyRayLimit_		= 100.0f;		//	レイの長さ
	DirectX::XMFLOAT3	hitPosition_		= {};			//	当たった位置
	DirectX::XMFLOAT3	hitNormal_			= {};			//	法線の方向
	std::string			hitMesh_			= {};			//	メッシュ名
	std::string			hitMaterial_		= {};			//	マテリアル名

};