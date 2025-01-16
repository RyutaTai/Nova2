#pragma once

#include "../Nova/AI/State.h"
#include "Player.h"
#include "../Nova/Others/JudgeTime.h"

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
		void DrawDebug()override;

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
		void DrawDebug()override;

	private:
		void PlayFootsteps(const float& elapsedTime);	//	足音再生

	private:
		float playFootstepsInterval_	= 0.278f;		//	足音SE再生間隔
		float footStepsTimer_			= 0.0f;			//	足音SE再生間隔用タイマー

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
		void DrawDebug()override;

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

//	コンボ01_1(右パンチ)
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
		void DrawDebug()override;

	private:
		bool JudgeAttackHit(const float& elapsedTime, const JudgeTime& animJudgeTime, const std::string& nodeName);
		bool JudgeInput(const JudgeTime& inputJudgeTime);	//	正しい入力が取れていたらtrue
		bool JudgeInputCommand(const JudgeTime& inputJudgeTime, const Command& command);
		void UpdateElapsedTime(const float& elapsedTime);	//	経過時間更新
		void UpdateAnimationSpeed();	//	アニメーション箇所で速度を変化

	private:
		JudgeTime	animJudgeTime_	= {};			//	判定を取るアニメーション区間
		JudgeTime	animSpeedChangeInterval_[3] = {};	//	アニメーション速度変化区間
		JudgeTime	cancellationTime_ = {};			//	キャンセル可能時間
		float		acceptInputFrame_ = 0.0f;		//	入力時間を受け付ける時間(CommandConfirm関数でさかのぼるフレーム数)
				
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
		void DrawDebug()override;

	private:
		bool JudgeAttackHit(const float& elapsedTime, const JudgeTime& animJudgeTime, const std::string& nodeName);
		bool JudgeInput(const JudgeTime& cancellationTime);
		bool JudgeInputCommand(const JudgeTime& inputJudgeTime, const Command& command);
		void UpdateElapsedTime(const float& elapsedTime);
		void UpdateAnimationSpeed();

	private:
		static constexpr int AnimJudgeCount = 2;	//	アニメーション判定区間の数
		JudgeTime			animJudgeTime_[AnimJudgeCount] = {};	//	判定を取るアニメーション区間
		JudgeTime			animSpeedChangeInterval_[4] = {};		//	判定を取るアニメーション区間
		JudgeTime			cancellationTime_ = {};		//	キャンセル可能時間
		float				acceptInputFrame_ = 0.0f;	//	入力時間を受け付ける範囲
		bool				isHit_ = false;				//	このコンボの最後の攻撃があたったらtrue

	};
}

//	コンボ01_3(右アッパー)
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
		void DrawDebug()override;

	private:
		bool JudgeAttackHit(const float& elapsedTime, const JudgeTime& animJudgeTime, const std::string& nodeName);
		bool JudgeInput(const JudgeTime& cancellationTime);	//	正しい入力が取れていたらtrue
		bool JudgeInputCommand(const JudgeTime& inputJudgeTime, const Command& command);
		void UpdateElapsedTime(const float& elapsedTime);

	private:
		static constexpr int AnimJudgeCount = 3;	//	アニメーション判定区間の数
		JudgeTime	animJudgeTime_[AnimJudgeCount] = {};		//	判定を取るアニメーション区間
		JudgeTime	cancellationTime_ = {};		//	キャンセル可能時間
		float		acceptInputFrame_ = {};		//	入力時間を受け付ける範囲
		bool		isHit_ = false;

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
		void DrawDebug()override;

	private:
		bool JudgeAttackHit(const float& elapsedTime, const JudgeTime& animJudgeTime, const std::string& nodeName);
		bool JudgeInput(const JudgeTime& cancellationTime);	//	正しい入力が取れていたらtrue
		bool JudgeInputCommand(const JudgeTime& inputJudgeTime, const Command& command);
		void UpdateElapsedTime(const float& elapsedTime);

	private:
		JudgeTime	animJudgeTime_	= {};		//	判定を取るアニメーション区間
		JudgeTime	cancellationTime_ = {};		//	キャンセル可能時間
		float		acceptInputFrame_ = {};		//	入力時間を受け付ける範囲
		bool		isHit_ = false;

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
		void DrawDebug()override;

	};
}
