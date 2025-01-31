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
		Pursuit,			//	追跡
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

	void Attack();

	//	----- 破棄処理 -----
	void JudgeDestroy()override;			//	破棄判定
	void Destroy()override;					//	破棄処理

	//	----- 弾丸 -----
	void LaunchBullet();										//	弾丸生成処理
	const float GetLaunchRange() const { return launchRange_; }	//	射程距離取得

	//	----- Collision -----
	void RegisterCollisionData()override;
	void UpdateCollisions(const float& elapsedTime)override;
	bool RayVsVertical(const float& elapsedTime)override;
	bool RayVsHorizontal(const float& elapsedTime)override;

	//	----- オーディオ -----
	void UpdateEmitter();		//	エミッター更新
	void UpdateAudioSource();	//	オーディオソース更新

	//	----- HP -----
	const int GetMaxHp()const { return MaxHp_; }		//	最大HP取得

	//	----- ステート -----
	void ChangeState(const StateType& state) { stateMachine_->ChangeState(static_cast<int>(state)); }	//	ステート遷移
	StateMachine<State<Drone>>* GetStateMachine() { return stateMachine_.get(); }	//	ステートマシン取得
	void DrawStateStr();	//	現在のステート文字列設定

	//	----- デバッグ描画 -----
	void DrawDebugPrimitive()override;			//	デバッグプリミティブ描画
	void DrawDebug()	override;				//	デバッグ描画

private:
	//	----- エフェクト -----
	std::shared_ptr <Effect>		effectResource_;	//	エフェクト
	float	effectScale_ = 1.0f;						//	エフェクトスケール
	
	// ----- 弾丸 -----
	float	launchTimer_ = 1.8f;	//	次の球を発射するまでのタイマー
	float	launchRange_ = 10.0f;	//	射程距離

	//	----- オーディオ -----
	SoundEmitter	emitter_ = {};				//	エミッター
	AudioSource3D*	sources_[static_cast<int>(Audio3D::Max)] = { nullptr };		//	オーディオソース
	AudioSource* debugSource_ = nullptr;

	//	----- ステート -----
	std::unique_ptr<StateMachine<State<Drone>>>	stateMachine_ = nullptr;		//	ステートマシン

private://	デバッグ用の変数
	// ----- DrawDebugPrimitive -----
	bool isCollisionSphere_ = true;		//	押し出し判定
	bool isAttackSphere_ = true;		//	攻撃判定
	bool isDamageSphere_ = false;		//	くらい判定

	bool	isBulletLaunch_	= true;		//	弾を発射するかどうか
	static const int MaxHp_ = 40;		//	最大HP
	//static const int MaxHp_ = 60;		//	最大HP
	//static const int MaxHp_ = 3;		//	最大HP

};

