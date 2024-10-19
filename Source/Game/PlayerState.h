#pragma once

#include "../Nova/AI/State.h"
#include "Player.h"

//	待機ステート
namespace PlayerState
{
	class IdleState :public State<Player>
	{
	public:
		IdleState(Player* owner) : State(owner) {}
		~IdleState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	};
}

//	移動ステート
namespace PlayerState
{
	class MoveState :public State<Player>
	{
	public:
		MoveState(Player* owner) : State(owner) {}
		~MoveState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	private:
		static constexpr float VELOCITY_SCALE_MAX = 30.0f;	//	
		static constexpr float WALK_TIMER_ADD = 3.0f;		//	walkTimer加算量
		static constexpr float WALK_TO_RUN_INTERVAL = 1.0f;	//	歩きから走りに切り替えるまでの時間

		float walkTimer_		= 0.0f;						//	歩き状態に入ってどのくらい経過したか
		float velocityScale_	= 1.0f;						//	Velocityに掛ける数

	};
}

//	攻撃ステート
namespace PlayerState
{
	class AttackState :public State<Player>
	{
	public:
		AttackState(Player* owner) : State(owner) {}
		~AttackState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

		void Attack();
		void PunchAttack(const float& elapsedTime);
		bool PuchVsBullet(const float& elapsedTime, const DirectX::XMFLOAT3& leftHandPos, const float leftHandRadius);
		bool PunchVsEnemy(const float& elpasedTime, const DirectX::XMFLOAT3& leftHandPos, const float leftHandRadius);
		bool PuchAttackCollision();
		void MoveTowardsEnemy(const float& elapsedTime);	//	ターゲットの方向へに向かって移動

		void SetTargetPosition(const DirectX::XMFLOAT3& pos) { targetPos_ = pos; }	//	ターゲット位置設定

	private:
		DirectX::XMFLOAT3 targetPos_ = {};	//	ターゲット
		float moveTime_ = 1.0f;				//	ターゲットへ向かってどのくらいの時間動くか
		bool isMove_ = false;				//	ターゲットへ移動中
		float judgeTime_ = 0.0f;

	};
}

//	回避ステート
namespace PlayerState
{
	class DodgeState :public State<Player>
	{
	public:
		DodgeState(Player* owner) : State(owner) {}
		~DodgeState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	};
}
