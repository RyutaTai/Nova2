#pragma once

#include <memory>

#include "Enemy.h"
#include "../Nova/Resources/Effect.h"
#include "BulletManager.h"
#include "../Nova/AI/StateMachine.h"
#include "../Nova/Audio/AudioManager.h"
#include "../Nova/Others/JudgeTime.h"

class Drone :public Enemy
{
public:
	enum class StateType	//	ステートの種類
	{
		Idle = 0,			//	待機
		Search, 			//	サーチ
		Move,				//	移動
		Attack,				//	攻撃
		Avoidance,			//	回避
		Max,				//	ステート最大数
	};

private:
	enum class Audio3D		//	3Dオーディオで鳴らしたい音
	{
		Shot,				//	発射音
		Move,				//	移動
		Destroy,			//	破壊音
		Bgm,				//	BGM
		Max,
	};

public:
	Drone();
	~Drone()override;

	void Initialize()override;
	void Update(const float& elapsedTime)override;
	void Render()override;

	bool RayVsVertical(const float& elapsedTime)override;
	bool RayVsHorizontal(const float& elapsedTime)override;
	void Move(const float& elapsedTime)override {}
	void Attack();
	void LaunchBullet();					//	弾丸生成処理
	void Turn(const float& elpasedTime);	//	旋回処理
	void Destroy()override;					//	破棄処理

	//	----- Collision -----
	void RegisterCollisionData()override;
	void UpdateCollisions(const float& elapsedTime)override;

	//	攻撃が当たったか判定する
	bool JudgeAttackHit(const float& elapsedTime, const JudgeTime& animJudgeTime, const DirectX::XMFLOAT3& attackPos, const float& radius)override;

	//	----- オーディオ -----
	void UpdateEmitter();		//	エミッター更新
	void UpdateAudioSource();	//	オーディオソース更新

	const int GetMaxHp()const { return MAX_HP; }		//	最大HP取得

	//	----- ステート -----
	void ChangeState(const StateType& state) { stateMachine_->ChangeState(static_cast<int>(state)); }	//	ステート遷移
	StateMachine<State<Drone>>* GetStateMachine() { return stateMachine_.get(); }	//	ステートマシン取得

	void DrawDebugPrimitive()override;			//	デバッグプリミティブ描画
	void DrawDebug()	override;				//	デバッグ描画

private:
	//	----- エフェクト -----
	std::shared_ptr <Effect>		effectResource_;							//	エフェクト
	float	effectScale_ = 1.0f;
	
	float	launchTimer_ = 1.8f;	//	次の球を発射するまでのタイマー
	
	//	----- オーディオ -----
	SoundEmitter	emitter_ = {};				//	エミッター
	AudioSource3D*	sources_[static_cast<int>(Audio3D::Max)] = { nullptr };		//	オーディオソース
	AudioSource* debugSource_ = nullptr;

	//	----- ステート -----
	std::unique_ptr<StateMachine<State<Drone>>>	stateMachine_ = nullptr;		//	ステートマシン

private://	デバッグ用の変数
	bool	bulletLaunch_	= true;		//	弾を発射するかどうか()
	static const int MAX_HP = 40;		//	最大HP
	//static const int MAX_HP = 60;		//	最大HP
	//static const int MAX_HP = 3;		//	最大HP

};

