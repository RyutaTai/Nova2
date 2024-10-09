#if 0
#include "StateDerived.h"

#include "MetaAI.h"
#include "../Others/MathHelper.h"
#include "../../Game/Player.h"

//	�T�[�`�X�e�[�g�f�X�g���N�^
SearchState<class T>::~SearchState()
{
	for (State<T>* state : subStatePool)
	{
		delete state;
	}
	subStatePool.clear();
}

//	�T�[�`�X�e�[�g�ɓ��������̃��\�b�h
void SearchState<class T>::Initialize()
{
	SetSubState(static_cast<int>(EnemySlime::Search::Idle));
}

//	�T�[�`�X�e�[�g�Ŏ��s���郁�\�b�h
void SearchState<class T>::Update(float elapsedTime)
{
	subState->Update(elapsedTime);
}

//	�T�[�`�X�e�[�g����o�Ă����Ƃ��̃��\�b�h
void SearchState<class T>::Finalize()
{

}

//	�o�g���X�e�[�g�f�X�g���N�^
BattleState<class T>::~BattleState()
{
	for (State* state : subStatePool)
	{
		delete state;
	}
	subStatePool.clear();
}

//	�o�g���X�e�[�g�ɓ��������̃��\�b�h
void BattleState<class T>::Initialize()
{
	SetSubState(static_cast<int>(EnemySlime::Battle::Attack));
}

//	�o�g���X�e�[�g�Ŏ��s���郁�\�b�h
void BattleState<class T>::Update(float elapsedTime)
{
	subState->Update(elapsedTime);
}

//	�o�g���X�e�[�g����o�Ă����Ƃ��̃��\�b�h
void BattleState<class T>::Finalize()
{

}

//	���V�[�u�X�e�[�g�̃f�X�g���N�^
RecieveState<class T>::~RecieveState()
{
	for (State* state : subStatePool)
	{
		delete state;
	}
	subStatePool.clear();
}

//	���V�[�u�X�e�[�g�ɓ��������̃��\�b�h
//void RecieveState<class T>::Initialize()
//{
//	//	�����X�e�[�g��ݒ�
//	SetSubState(static_cast<int>(EnemySlime::Recieve::Called));
//}

template<class T>
void RecieveState<T>::Initialize()
{
	owner_->SearchPlayer();
}

//	���V�[�u�X�e�[�g�Ŏ��s���郁�\�b�h
void RecieveState<class T>::Update(float elapsedTime)
{
	//	�q�X�e�[�g���s
	subState->Update(elapsedTime);
	//	�v���C���[���G
	if (owner_->SearchPlayer())
	{
		//	Battle�X�e�[�g�֑J��
		owner_->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Battle));
	}
}

//	���V�[�u�X�e�[�g����o�Ă����Ƃ��̃��\�b�h
void RecieveState<class T>::Finalize()
{

}

//	�p�j�X�e�[�g�ɓ��������̃��\�b�h
void WanderState<class Enemy>::Initizalize()
{
	owner_->SetRandomTargetPosition();
	owner_->SetAnimation(static_cast<int>(EnemyAnimation::RunFWD), true);
}

//	�p�j�X�e�[�g�Ŏ��s���郁�\�b�h
void WanderState<class Enemy>::Update(float elapsedTime)
{
	//	�ړI�n�_�܂ł�XZ���ʂł̋�������
	DirectX::XMFLOAT3 position = owner_->GetTransform()->GetPosition();
	DirectX::XMFLOAT3 targetPosition = owner_->GetTargetPosition();
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;

	//	�ړI�n�֒�����
	float radius = owner_->GetRadius();
	if (distSq < radius * radius)
	{
		//	�ҋ@�X�e�[�g�֑J��
		//	ChangeSubState�N���X��2�w�ڂ�State��؂�ւ���
		owner_->GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Search::Idle));
	}
	//	�ړI�n�_�ֈړ�
	owner_->MoveToTarget(elapsedTime, 0.5f);
	//	�v���C���[���G
	if (owner_->SearchPlayer())
	{
		//	����������ǐՃX�e�[�g�֑J��
		//	ChangeState�N���X��1�w�̖�State��؂�ւ���
		owner_->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Battle));
		owner_->GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Battle::Pursuit));
	}

}

//	�p�j�X�e�[�g����o�Ă����Ƃ��̃��\�b�h
void WanderState<class T>::Finalize()
{

}

//	�ҋ@�X�e�[�g�ɓ��������̃��\�b�h
void IdleState<class Enemy>::Initizalize()
{
	owner_->SetAnimation(static_cast<int>(EnemyAnimation::IdleNormal), true);
	//	�^�C�}�[�������_���ݒ�
	owner_->SetStateTimer(Mathf::RandomRange(3.0f, 5.0f));
}

//	�ҋ@�X�e�[�g�Ŏ��s���郁�\�b�h
void IdleState<class T>::Update(float elapsedTime)
{
	//	�^�C�}�[����
	owner_->SetStateTimer(owner_->GetStateTimer() - elapsedTime);

	//	�ҋ@���Ԃ��o�߂����Ƃ��p�j�X�e�[�g�֑J��
	if (owner_->GetStateTimer() < 0.0f)
	{
		owner_->GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Search::Wander));
	}

	//	�v���C���[�����������Ƃ��ǐՃX�e�[�g�֑J��
	if (owner_->SearchPlayer())
	{
		owner_->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Battle));
		owner_->GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Battle::Pursuit));
	}

}

//	�ҋ@�X�e�[�g����o�Ă����Ƃ��̃��\�b�h
void IdleState<class T>::Finalize()
{

}

//	�ǐՃX�e�[�g�ɓ��������̃��\�b�h
void PursuitState<class T>::Initialize()
{
	owner_->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::RunFWD), true);
	//	���b�ԒǐՂ���^�C�}�[�������_���ݒ�
	owner_->SetStateTimer(Mathf::RandomRange(3.0f, 5.0f));
	//	�G�𔭌��������ߒ��Ԃ��Ă�
	//	�G�l�~�[���烁�^AI��MsgCallHelp�𑗐M����B
	Meta::Instance().SendMessaging(owner_->GetId(), (int)Meta::Identity::Meta, MESSAGE_TYPE::MsgCallHelp);
}

//	�ǐՃX�e�[�g�Ŏ��s���郁�\�b�h
void PursuitState<class T>::Update(float elapsedTime)
{
	//	�ڕW�n�_���v���C���[�ʒu�ɐݒ�
	owner_->SetTargetPosition(Player::Instance().GetTransform()->GetPosition());

	//	�ړI�n�_�ֈړ�
	owner_->MoveToTarget(elapsedTime, 1.0);

	//	�^�C�}�[����
	owner_->SetStateTimer(owner_->GetStateTimer() - elapsedTime);

	//	�ǐՎ��Ԃ��o�߂����Ƃ��ҋ@�X�e�[�g�֑J��
	if (owner_->GetStateTimer() < 0.0f)
	{
		owner_->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Search));
	}

	float vx = owner_->GetTargetPosition().x - owner_->GetTransform()->GetPosition().x;
	float vy = owner_->GetTargetPosition().y - owner_->GetTransform()->GetPosition().y;
	float vz = owner_->GetTargetPosition().z - owner_->GetTransform()->GetPosition().z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	//	�U���͈͂ɓ������Ƃ��U���X�e�[�g�֑J��
	if (dist < owner_->GetAttackRange())
	{
		owner_->GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Battle::Attack));
	}
}

//	�ǐՃX�e�[�g����o�Ă����Ƃ��̃��\�b�h
void PursuitState<class T>::Finalize()
{

}

//	�U���X�e�[�g�ɓ��������̃��\�b�h
void AttackState<class T>::Initialize()
{
	//	�U�������Ȃ����
	if (!owner_->GetAttackFlg())
	{
		//	�U���������^AI���狁�߂�
		//	�G�l�~�[���烁�^AI��MsgAskAttackRight�𑗐M����
		Meta::Instance().SendMessaging(owner_->GetId(), (int)Meta::Identity::Meta, MESSAGE_TYPE::MsgAskAttackRight);
	}
	//	�U����������΃��[�V�����Đ��J�n
	if (owner_->GetAttackFlg())
	{
		owner_->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::Attack01), false);
	}
}

//	�U���X�e�[�g�Ŏ��s���郁�\�b�h
void AttackState<class T>::Update(float elapsedTime)
{
	//	�U����������Ƃ� 
	if (owner_->GetAttackFlg())
	{
		//	�U�����[�V�������I����Ă���ΐ퓬���ҋ@�֑J�� 
		if (!owner_->GetModel()->IsPlayAnimation())
		{
			owner_->GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Battle::Standby));
		}
	}
	else
	{
		//	�U�������Ȃ��Ƃ��X�e�[�g�ύX 
		owner_->GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Battle::Standby));
	}
}

//	�U���X�e�[�g����o�Ă����Ƃ��̃��\�b�h
void AttackState<class T>::Finalize()
{
	if (owner_->GetAttackFlg())
	{
		//	�U�����I������Ƃ��U�����̔j��
		//	�U������false�ɐݒ�
		owner_->SetAttackFlg(false);
		//	�G�l�~�[���烁�^AI��MsgChangeAttackRight�𑗐M����
		Meta::Instance().SendMessaging(owner_->GetId(), (int)Meta::Identity::Meta, MESSAGE_TYPE::MsgChangeAttackRight);
	}
}

//	�R�[���h�X�e�[�g�ɓ��������̃��\�b�h
void CalledState<class T>::Initialize()
{
	owner_->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::WalkBWD), true);
	owner_->SetStateTimer(5.0f);
}

//	�R�[���h�X�e�[�g�Ŏ��s���郁�\�b�h
void CalledState<class T>::Update(float elapsedTime)
{
	//	�^�C�}�[�Ǘ�
	float timer = owner_->GetStateTimer();
	timer -= elapsedTime;
	owner_->SetStateTimer(timer);

	if (timer < 0.0f)
	{
		//	�p�j�X�e�[�g�֑J��
		owner_->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Search));
	}
	//	�Ώۂ��v���C���[�n�_�ɐݒ�
	owner_->SetTargetPosition(Player::Instance().GetTransform()->GetPosition());
	owner_->MoveToTarget(elapsedTime, 1.0f);
}

//	�R�[���h�X�e�[�g����o�Ă����Ƃ��̃��\�b�h
void CalledState<class T>::Finalize()
{

}

//	�퓬�ҋ@�X�e�[�g�ɓ��������̃��\�b�h
void StandbyState<class T>::Initialize()
{
	owner_->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::IdleBattle), false);
}

//	�퓬�ҋ@�X�e�[�g�Ŏ��s���郁�\�b�h
void StandbyState<class T>::Update(float elapsedTime)
{
	//	�U����������Ƃ�
	if (owner_->GetAttackFlg())
	{
		//	�X�e�[�g�ύX
		owner-_>GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Battle::Attack));
	}
	//	�ڕW�n�_���v���C���[�ʒu�ɐݒ�
	owner_->SetTargetPosition(Player::Instance().GetTransform()->GetPosition());
	float vx = owner_->GetTargetPosition().x - owner_->GetPosition().x;
	float vy = owner_->GetTargetPosition().y - owner_->GetPosition().y;
	float vz = owner_->GetTargetPosition().z - owner_->GetPosition().z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);
	//	�U���͈͂���o����ǐՃX�e�[�g�֑J��
	if (dist > owner_->GetAttackRange())
	{
		owner_->GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Battle::Pursuit));
	}
}

//	�퓬�ҋ@�X�e�[�g����o�Ă����Ƃ��̃��\�b�h
void StandbyState<class T>::Finalize()
{

}
#endif