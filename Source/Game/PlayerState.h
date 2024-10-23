#pragma once

#include "../Nova/AI/State.h"
#include "Player.h"
#include "JudgeTime.h"

//	�ҋ@�X�e�[�g
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

//	�ړ��X�e�[�g
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
		static constexpr float walkTimerAdd_ = 10.0f;		//	walkTimer���Z��
		static constexpr float walkoToRunInterval_ = 0.5f;	//	�������瑖��ɐ؂�ւ���܂ł̎���
		static constexpr float velocityAdd_ = 0.1f;			//	velocityScale���Z��		
		float walkTimer_		= 0.0f;						//	������Ԃɓ����Ăǂ̂��炢�o�߂�����
		float velocityScale_	= 3.0f;						//	Velocity�Ɋ|����W��

	};
}

//	�U���X�e�[�g
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
		void MoveTowardsEnemy(const float& elapsedTime);	//	�^�[�Q�b�g�̕����ւɌ������Ĉړ�

		void SetTargetPosition(const DirectX::XMFLOAT3& pos) { targetPos_ = pos; }	//	�^�[�Q�b�g�ʒu�ݒ�


	private:
		DirectX::XMFLOAT3 targetPos_ = {};	//	�^�[�Q�b�g
		float moveTime_ = 1.0f;				//	�^�[�Q�b�g�֌������Ăǂ̂��炢�̎��ԓ�����
		bool isMove_ = false;				//	�^�[�Q�b�g�ֈړ���
		float judgeTimer_ = 0.0f;			//	���肷�鎞�Ԃ𐧌�����Ƃ��Ɏg�p

	};
}

//	�R���{01_1
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

//	�R���{01_2
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
		enum class JudgePart	//	�����蔻�蕔��
		{
			LeftHand = 0,		//	���W���u
			RightHand,			//	�E�A�b�p�[
		};

		JudgeTime judgeTimes_[2] = {};

	};
}

//	�R���{01_3
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

//	�R���{01_4
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

//	����X�e�[�g
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
