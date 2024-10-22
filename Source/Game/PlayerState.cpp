#include "PlayerState.h"

#include <algorithm>

#include "../Nova/Debug/DebugRenderer.h"
#include "../Nova/Graphics/Graphics.h"
#include "../Nova/Collision/Collision.h"
#include "../Nova/Others/MathHelper.h"
#include "BulletManager.h"
#include "EnemyManager.h"
#include "../Nova/Input/GamePad.h"
#include "../Nova/Input/Input.h"

//	�ҋ@�X�e�[�g
namespace PlayerState
{
	void IdleState::Initialize()
	{
		//	�A�j���[�V�����Z�b�g
		owner_->PlayAnimation(Player::AnimationType::Idle, true, 1.0f, 0.4f);
	}

	void IdleState::Update(const float& elapsedTime)
	{
		//	�ړ����͂�����΁A�ړ��X�e�[�g�֑J��
		if (owner_->InputMove(elapsedTime))
		{
			owner_->ChangeState(Player::StateType::Move);
			//owner_->PlayAnimation(Player::AnimationType::ANIM_WALK);
		 return;
		}
		
		//	�U���X�e�[�g�֑J��
		if (owner_->GetCombo0ButtonDown())
		{
			owner_->ChangeState(Player::StateType::ComboOne1);
			return;
		}

	}

	void IdleState::Finalize()
	{

	}
}

//	�ړ��X�e�[�g
namespace PlayerState
{
	void MoveState::Initialize()
	{
#if 0
		//	�A�j���[�V�����Z�b�g
		owner_->PlayBlendAnimation(Player::AnimationType::ANIM_WALK, true);

		owner_->SetWeight(0.5f);
#endif
		//	�A�j���[�V�����Z�b�g
		owner_->PlayAnimation(Player::AnimationType::Walk , true, 1.0f, 0.2f);

		//	�X�V�����Ɏg���ϐ�������
		walkTimer_ = 0.0f;
		velocityScale_ = 1.0f;
	}

	void MoveState::Update(const float& elapsedTime)
	{
#if 0
		//	�A�j���[�V������weight�l�����Z
		owner_->AddWeight(elapsedTime);
#endif
		//	���ȏ�̎��Ԃ��o�߂����瑖�胂�[�V�����ֈڍs
		walkTimer_ += walkTimerAdd_ * elapsedTime;
		owner_->MultiplyVelocityXZ(velocityScale_, elapsedTime);
		if (walkTimer_ > walkoToRunInterval_ && owner_->GetCurrentAnimType() != Player::AnimationType::Run)
		{
			owner_->PlayAnimation(Player::AnimationType::Run, true, 1.0f, 0.2f);
		}
		else velocityScale_ += velocityAdd_ * elapsedTime;

		//	�ړ����͂��Ȃ��Ȃ�����ҋ@�X�e�[�g�֑J��
		if (!owner_->InputMove(elapsedTime))
		{
			//	�ҋ@�X�e�[�g�֑J��
			owner_->GetStateMachine()->ChangeState(static_cast<int>(Player::StateType::Idle));
			return;
		}
		//	�U���X�e�[�g�֑J��
		//owner_->TransitionAttack();

		//	�ړ��X�s�[�h���Z
		owner_->AddMoveSpeed(velocityScale_, elapsedTime);

	}

	void MoveState::Finalize()
	{
		owner_->SetMoveSpeed(2.0f);
	}
}

//	�U���X�e�[�g
namespace PlayerState
{
	void AttackState::Initialize()
	{
#if 0
		//	�A�j���[�V�����Z�b�g
		owner_->PlayBlendAnimation(Player::AnimationType::ANIM_PUNCH, false, 2.0f);

		//	weight�l�Z�b�g
		//owner_->SetWeight(0.0f);
		owner_->SetWeight(1.0f);

		//	�u�����h���ݒ�
		owner_->SetBlendRate(1.0f);	//	����?
#endif
		//	�A�j���[�V�����Z�b�g
		//owner_->PlayAnimation(Player::AnimationType::Combo0_1, false, 2.0f, 0.0f);
		owner_->PlayAnimation(Player::AnimationType::Combo0_1, false, 1.0f, 0.0f);

		//	�����蔻��^�C�}�[������
		judgeTimer_ = 0.0f;
	}

	void AttackState::Update(const float& elapsedTime)
	{
		//	weight���Z
		//owner_->AddWeight(elapsedTime);

		//	�A�j���[�V�����X�V����
		//owner_->UpdateBlendAnimation(elapsedTime);

#if 1
		//	�A�j���[�V�����Đ����I�������ҋ@�X�e�[�g�֑J��
		if (owner_->IsPlayAnimation() == false /* && isMove_ == false*/)
		{
			owner_->GetStateMachine()->ChangeState(static_cast<int> (Player::StateType::Idle));
			return;
		}
#endif
		//	�U������
		//PunchAttack(elapsedTime, "SKM_Manny_LOD0", "ik_hand_gun");

		//	����p�^�C�}�[�X�V
		UpdateJudgeTimer(elapsedTime);

	}

	//	���ōU���i���ʂ̍U���j
	
	//bool AttackState::PunchAttack(const float& elapsedTime, const std::string& meshName, const std::string& boneName)
	//{
	//	bool isHIt = false;
	//
	//	//	�E��̃��[���h���W�擾
	//	DirectX::XMFLOAT4X4 world;
	//	DirectX::XMStoreFloat4x4(&world, owner_->GetTransform()->CalcWorld());	//	�v���C���[�̃��[���h�s��
	//	//DirectX::XMStoreFloat4x4(&world, owner_->GetTransform()->CalcWorldMatrix(scale));	//	�v���C���[�̃��[���h�s��
	//	DirectX::XMFLOAT3 leftHandPos = owner_->GetJointPosition(meshName, boneName, world);
	//
	//	//	�����蔻��p�̔��a�Z�b�g
	//	float leftHandRadius = 3.0f;
	//
	//	//	�Փ˔���p�̃f�o�b�O����`��
	//	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
	//	//debugRenderer->DrawSphere(leftHandPos, leftHandRadius, DirectX::XMFLOAT4(1, 1, 1, 1));
	//
	//	//	�e�ۂւ̍U������
	//	if (PuchVsBullet(elapsedTime, leftHandPos, leftHandRadius) == true)isHIt = true;
	//
	//	//	�G�ւ̓����蔻��
	//	if (PunchVsEnemy(elapsedTime, leftHandPos, leftHandRadius) == true)isHIt = true;
	//
	//	return isHIt;
	//}

	//	���ƒe�ۂ̓����蔻��
	bool AttackState::PuchVsBullet(const float& elapsedTime, const DirectX::XMFLOAT3& leftHandPos, const float leftHandRadius)
	{
		bool isHitBullet = false;
		BulletManager& bulletManager = BulletManager::Instance();
		for (int bulletNum = 0; bulletNum < bulletManager.GetBulletCount(); ++bulletNum)
		{
			//	�e�ۂƉE��Ƃ̓����蔻��
			Bullet* bullet = bulletManager.GetBullet(bulletNum);
			DirectX::XMFLOAT3	bulletPos = bullet->GetTransform()->GetPosition();	//	�e�ۂ̈ʒu
			float				bulletRadius = bullet->GetRadius();					//	�e�ۂ̔��a
			DirectX::XMFLOAT3	outPos = {};
			if (Collision::IntersectSphereVsSphere(leftHandPos, leftHandRadius, bulletPos, bulletRadius, outPos))
			{
				isHitBullet = true;
			}

			//	�e�ۂɌ����������Ă�����
			if (isHitBullet)
			{
				//	�e�ۂ̃_���[�W�t���O�ݒ�
				bulletManager.GetBullet(bulletNum)->SetDamaged(isHitBullet);

				// TODO:�e�ۂ�ł��Ă����G�̕��֐i�ޏ���
				SetTargetPosition(bullet->GetOwnerPosition());
				MoveTowardsEnemy(elapsedTime);

			}
		}
		return isHitBullet;

	}

	//	���ƓG�̓����蔻��
	bool AttackState::PunchVsEnemy(const float& elapsedTime, const DirectX::XMFLOAT3& leftHandPos, const float leftHandRadius)
	{
		DirectX::XMFLOAT3 outPosition = {};
		bool isPunchHitEnemy = false;

		for (Enemy* enemy : EnemyManager::Instance().GetEnemies())
		{
			DirectX::XMFLOAT3 ePos = enemy->GetTransform()->GetPosition();
			float eRadius = enemy->GetRadius() + 0.1f;
			float eHeight = enemy->GetHeight() * 2;
			DirectX::XMFLOAT3 ePosOffset = { 0.0f,-eHeight / 2.0f,0.0f };

			//	���Ɖ~���œ����蔻��
			//if (1.1f/2.0f < judgeTime_ && judgeTime_ < 1.2f/2.0f)
			//{
			if (Collision::IntersectSphereVsCylinder(leftHandPos, leftHandRadius, ePos + ePosOffset, eRadius, eHeight, outPosition))
			{
				enemy->SubtractHp(1);
				float effectScale = 50.0f;
				isPunchHitEnemy = true;
			}
			else isPunchHitEnemy = false;

			//	�G�t�F�N�g�Đ��ݒ�(Player��Render()�ŕ`�悳���)
			if (isPunchHitEnemy)
			{
				isPunchHitEnemy = true;
				owner_->SetPlayEffectFlag(true);
				owner_->SetEffectPos(leftHandPos);
			}
			//}
		}
		return isPunchHitEnemy;
	}

	//	�G�̕����ւɌ������Ĉړ�
	void AttackState::MoveTowardsEnemy(const float& elapsedTime)
	{
		isMove_ = true;
		DirectX::XMVECTOR Velocity = {};
		DirectX::XMVECTOR PlayerPos = DirectX::XMLoadFloat3(&owner_->GetTransform()->GetPosition());	//	�v���C���[�̈ʒu
		DirectX::XMVECTOR TargetPos = DirectX::XMLoadFloat3(&targetPos_);								//	�G�̈ʒu
		DirectX::XMVECTOR Move = DirectX::XMVectorSubtract(TargetPos, PlayerPos);						//	�v���C���[����G�Ɍ������x�N�g��

		//	XZ���ʂ݂̂̈ړ��ɂ���
		Move = DirectX::XMVectorSetY(Move, 0.0f);

		//	���񂾂�ƈړ�����
		float moveTimer = moveTime_;
		Velocity = DirectX::XMVectorLerp(Velocity, Move, moveTimer);

		//	�ړ��^�C�}�[�X�V
		moveTimer -= elapsedTime;
		if (moveTimer < 0.0f)
		{
			isMove_ = false;
		}
		DirectX::XMFLOAT3 velocity;
		DirectX::XMStoreFloat3(&velocity, Velocity);

		//	�v���C���[�̃x���V�e�B�ɉ��Z
		//owner_->AddVelocity(move);
		owner_->SetVelocity(velocity);

		//	�ړ�����
		//owner_->Move();

	}

	void AttackState::Finalize()
	{
		judgeTimer_ = 0.0f;
	}

}

//	�R���{01_1
namespace PlayerState
{
	void ComboOne1::Initialize()
	{
		//	�A�j���[�V�����Z�b�g
		//owner_->PlayAnimation(Player::AnimationType::Combo0_1, false, 1.0f, 0.0f, 0.9f);
		owner_->PlayAnimation(Player::AnimationType::Combo0_1, false, 1.0f, 0.0f, 0.0f);

		//	���[�g���[�V����
		owner_->SetUseRootMotion(true);

		//	���莞�ԃZ�b�g
		judgeTime_.SetMinJudgeTime(0.55f);
		judgeTime_.SetMaxJudgeTime(0.735f);

	}

	void ComboOne1::Update(const float& elpasedTime)
	{
		// TODO:�A�j���[�V�����̒�������
		if (owner_->IsPlayAnimation() == false)
		//if (owner_->GetCurrentAnimationSeconds() >= 1.0f)
		{
			owner_->ChangeState(Player::StateType::Idle);
			//owner_->ChangeState(Player::StateType::ComboOne2);
			return;
		}
	}

	void ComboOne1::Finalize()
	{
		owner_->SetUseRootMotion(false);
	}

}

//	�R���{01_2
namespace PlayerState
{
	void ComboOne2::Initialize()
	{
		//	�A�j���[�V�����Z�b�g
		owner_->PlayAnimation(Player::AnimationType::Combo0_2, false, 1.0f, 0.0f);

		//	���[�g���[�V����
		owner_->SetUseRootMotion(true);

		//	���莞�ԃZ�b�g


	}

	void ComboOne2::Update(const float& elpasedTime)
	{
		if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::ComboOne3);
			return;
		}
	}

	void ComboOne2::Finalize()
	{
		owner_->SetUseRootMotion(false);
	}

}

//	�R���{01_3
namespace PlayerState
{
	void ComboOne3::Initialize()
	{
		//	�A�j���[�V�����Z�b�g
		owner_->PlayAnimation(Player::AnimationType::Combo0_3, false, 1.0f, 0.0f);

		//	���[�g���[�V����
		owner_->SetUseRootMotion(true);

		//	

	}

	void ComboOne3::Update(const float& elpasedTime)
	{
		if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::ComboOne4);
			return;
		}
	}

	void ComboOne3::Finalize()
	{
		owner_->SetUseRootMotion(false);
	}

}

//	�R���{01_4
namespace PlayerState
{
	void ComboOne4::Initialize()
	{
		//	�A�j���[�V�����Z�b�g
		owner_->PlayAnimation(Player::AnimationType::Combo0_4, false, 1.0f, 0.0f);

		//	���[�g���[�V����
		owner_->SetUseRootMotion(true);

		//	

	}

	void ComboOne4::Update(const float& elpasedTime)
	{
		if (owner_->IsPlayAnimation() == false)
		{
			owner_->ChangeState(Player::StateType::Idle);
			return;
		}

	}

	void ComboOne4::Finalize()
	{
		owner_->SetUseRootMotion(false);
	}

}

//	����X�e�[�g
namespace PlayerState
{
	void DodgeState::Initialize()
	{
		owner_->PlayAnimation(Player::AnimationType::DodgeFront, false, 1.0f, 0.0f);
	}

	void DodgeState::Update(const float& elapsedTime)
	{

	}

	void DodgeState::Finalize()
	{

	}
}
