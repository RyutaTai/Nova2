#pragma once

#include "../Nova/AI/State.h"
#include "Player.h"

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
		static constexpr float VELOCITY_SCALE_MAX = 30.0f;	//	
		static constexpr float WALK_TIMER_ADD = 3.0f;		//	walkTimer���Z��
		static constexpr float WALK_TO_RUN_INTERVAL = 1.0f;	//	�������瑖��ɐ؂�ւ���܂ł̎���

		float walkTimer_		= 0.0f;						//	������Ԃɓ����Ăǂ̂��炢�o�߂�����
		float velocityScale_	= 1.0f;						//	Velocity�Ɋ|���鐔

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

		void Attack();
		void PunchAttack(const float& elapsedTime);
		bool PuchVsBullet(const float& elapsedTime, const DirectX::XMFLOAT3& leftHandPos, const float leftHandRadius);
		bool PunchVsEnemy(const float& elpasedTime, const DirectX::XMFLOAT3& leftHandPos, const float leftHandRadius);
		bool PuchAttackCollision();
		void MoveTowardsEnemy(const float& elapsedTime);	//	�^�[�Q�b�g�̕����ւɌ������Ĉړ�

		void SetTargetPosition(const DirectX::XMFLOAT3& pos) { targetPos_ = pos; }	//	�^�[�Q�b�g�ʒu�ݒ�

	private:
		DirectX::XMFLOAT3 targetPos_ = {};	//	�^�[�Q�b�g
		float moveTime_ = 1.0f;				//	�^�[�Q�b�g�֌������Ăǂ̂��炢�̎��ԓ�����
		bool isMove_ = false;				//	�^�[�Q�b�g�ֈړ���
		float judgeTime_ = 0.0f;

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
