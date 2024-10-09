#pragma once

#include "Character.h"
//#include "../Nova/AI/StateMachine.h"
//#include "../Nova/AI/Telegram.h"

class Enemy :public Character
{
protected:
	enum class ENEMY_TYPE		//	Enemy�̎��
	{
		DRONE = 0,
		DRAGONKIN,
		MAX,
	};
	ENEMY_TYPE myType_;

	enum class STATE	//	�X�e�[�g
	{
		Search,
		Battle,
		Recieve,		//	MetaAI���烁�b�Z�[�W����M�����Ƃ��̃X�e�[�g
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
		Standby,		//	�퓬���ɍU�����������Ă��Ȃ��Ƃ��̏���
	};

	enum class Recieve	//	MetaAI���o�R���đ��̃G�l�~�[����Ă΂ꂽ�Ƃ��̏���
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

	virtual bool				SearchPlayer();																	//	�v���C���[���G
	//virtual bool OnMessage(const Telegram& msg);																//	���b�Z�[�W��M�֐�
	virtual void				Destroy();

	ENEMY_TYPE					GetMyType()										{ return myType_; }				//	�G�̎�ގ擾
	//StateMachine <Enemy>*	GetStateMachine()	{ return stateMachine_; }
	DirectX::XMFLOAT3			GetTargetPosition()								{ return targetPosition_; }		//	�^�[�Q�b�g�|�W�V�����擾
	float						GetStateTimer()									{ return stateTimer_; }			//	�X�e�[�g�^�C�}�[�擾

	void						SetDamaged(bool damaged) { damaged_ = damaged; }		//	�_���[�W�t���O�ݒ�
	void						SetMyType(ENEMY_TYPE myType)					{ myType_ = myType; }			//	�����̎�ސݒ�
	virtual void				SetRandomTargetPosition();														//	�^�[�Q�b�g�ʒu�������_���ݒ�
	void						SetTargetPosition(DirectX::XMFLOAT3 position)	{ targetPosition_ = position; }	//	�^�[�Q�b�g�|�W�V�����ݒ�
	void						SetStateTimer(float timer)						{ stateTimer_ = timer; }		//	�X�e�[�g�^�C�}�[�ݒ�


protected:
	//StateMachine <Enemy>*		stateMachine_	= nullptr;								//	�X�e�[�g�}�V��

	DirectX::XMFLOAT3			targetPosition_		=	{ 0.0f,0.0f,0.0f };				//	�^�[�Q�b�g�ʒu
	DirectX::XMFLOAT3			territoryOrigin_	=	{ 0.0f,0.0f,0.0f };				//	���G�͈͂̌��_
	float						territoryRange_		=	10.0f;							//	���G�͈�
	float						searchRange_		=	50.0f;							//	���G����
	float						stateTimer_			=	0.0f;							//
	bool						damaged_			=	false;							//	�U�����󂯂����ǂ���

};

