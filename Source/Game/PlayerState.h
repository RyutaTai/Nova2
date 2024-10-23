#pragma once

#include "../Nova/AI/State.h"
#include "Player.h"
#include "JudgeTime.h"

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
		static constexpr float velocityScaleMax_ = 30.0f;	//	
		static constexpr float walkTimerAdd_ = 10.0f;		//	walkTimer加算量
		static constexpr float walkoToRunInterval_ = 0.5f;	//	歩きから走りに切り替えるまでの時間
		static constexpr float velocityAdd_ = 0.1f;			//	velocityScale加算量		
		float walkTimer_		= 0.0f;						//	歩き状態に入ってどのくらい経過したか
		float velocityScale_	= 3.0f;						//	Velocityに掛ける係数

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

		void UpdateJudgeTimer(const float& elapsedTime) { judgeTimer_ += elapsedTime; }
		//bool PunchAttack(const float& elapsedTime, const std::string& meshName, const std::string& boneName);
		bool PuchVsBullet(const float& elapsedTime, const DirectX::XMFLOAT3& leftHandPos, const float leftHandRadius);
		bool PunchVsEnemy(const float& elpasedTime, const DirectX::XMFLOAT3& leftHandPos, const float leftHandRadius);
		bool PuchAttackCollision();
		void MoveTowardsEnemy(const float& elapsedTime);	//	ターゲットの方向へに向かって移動

		void SetTargetPosition(const DirectX::XMFLOAT3& pos) { targetPos_ = pos; }	//	ターゲット位置設定


	private:
		DirectX::XMFLOAT3 targetPos_ = {};	//	ターゲット
		float moveTime_ = 1.0f;				//	ターゲットへ向かってどのくらいの時間動くか
		bool isMove_ = false;				//	ターゲットへ移動中
		float judgeTimer_ = 0.0f;			//	判定する時間を制限するときに使用

	};
}

//	コンボ01_1
namespace PlayerState
{
	class ComboOne1 :public State<Player>
	{
	public:
		ComboOne1(Player* owner) :State(owner) {}
		~ComboOne1(){}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	private:
		JudgeTime judgeTime_ = {};

	};
}

//	コンボ01_2
namespace PlayerState
{
	class ComboOne2 :public State<Player>
	{
	public:
		ComboOne2(Player* owner) :State(owner) {}
		~ComboOne2() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	private:
		bool IsHitRightHand(const float& elapsedTime);
		bool IsHitLeftHand(const float& elapsedTime);

	private:
		enum class JudgePart	//	当たり判定部分
		{
			LeftHand = 0,		//	左ジャブ
			RightHand,			//	右アッパー
		};

		JudgeTime judgeTimes_[2] = {};

	};
}

//	コンボ01_3
namespace PlayerState
{
	class ComboOne3 :public State<Player>
	{
	public:
		ComboOne3(Player* owner) :State(owner) {}
		~ComboOne3() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	private:

	};
}

//	コンボ01_4
namespace PlayerState
{
	class ComboOne4 :public State<Player>
	{
	public:
		ComboOne4(Player* owner) :State(owner) {}
		~ComboOne4() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

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
