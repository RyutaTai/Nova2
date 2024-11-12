#pragma once

#include "Character.h"
//#include "../Nova/AI/StateMachine.h"
//#include "../Nova/AI/Telegram.h"

class Enemy :public Character
{
protected:
	enum class EnemyType		//	Enemyの種類
	{
		Drone = 0,
		Dragonkin,
		Max,
	};
	EnemyType myType_;

	enum class STATE	//	ステート
	{
		Search,
		Battle,
		Recieve,		//	MetaAIからメッセージを受信したときのステート
	};

	enum class Search
	{
		Idle,
		Wander
	};

	enum class Battle
	{
		Pursuit,
		Attack,
		Standby,		//	戦闘中に攻撃権を持っていないときの処理
	};

	enum class Recieve	//	MetaAIを経由して他のエネミーから呼ばれたときの処理
	{
		Called,
	};

public:
	Enemy(const std::string& fileName, const std::string& rootNodeName = "root");
	~Enemy()override {}

	virtual void				Initialize()	override = 0;
	virtual void				Update(const float& elapsedTime)		override = 0;
	virtual void				Render()		override = 0;
	virtual void				DrawDebug()		override = 0;
	virtual void				DrawDebugPrimitive() = 0;
	virtual bool				SearchPlayer();																//	プレイヤー索敵
	//virtual bool OnMessage(const Telegram& msg);															//	メッセージ受信関数
	virtual void				Destroy();

	void						SetDamaged(const bool& damaged)		{ damaged_ = damaged; }					//	ダメージフラグ設定
	void						SetMyType(const EnemyType& myType)	{ myType_ = myType; }					//	自分の種類設定
	virtual void				SetRandomTargetPosition();													//	ターゲット位置をランダム設定
	void						SetTargetPosition(const DirectX::XMFLOAT3& position) { targetPosition_ = position; }	//	ターゲットポジション設定
	void						SetRunTimer(const float& timer) { runTimer_ = timer; }			//	ステートタイマー設定

	EnemyType					GetMyType()										{ return myType_; }				//	敵の種類取得
	DirectX::XMFLOAT3			GetTargetPosition()								{ return targetPosition_; }		//	ターゲットポジション取得
	float						GetRunTimer()									{ return runTimer_; }			//	ステートタイマー取得

protected:
	DirectX::XMFLOAT3			targetPosition_		=	{ 0.0f,0.0f,0.0f };				//	ターゲット位置
	DirectX::XMFLOAT3			territoryOrigin_	=	{ 0.0f,0.0f,0.0f };				//	索敵範囲の原点
	float						territoryRange_		=	10.0f;							//	索敵範囲
	float						searchRange_		=	50.0f;							//	索敵距離
	float						runTimer_			=	0.0f;							//
	bool						damaged_			=	false;							//	攻撃を受けたかどうか

};

