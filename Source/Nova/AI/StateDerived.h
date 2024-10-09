#pragma once
#if 0
#include "StateMachine.h"
#include "../../Game/Enemy.h"

//	�T�[�`�X�e�[�g
template<class T>
class SearchState :public HierarchicalState<Enemy>
{
public:
	SearchState(T* owner) :HierarchicalState<T>(owner) {}
	~SearchState();

	void Initialize();
	void Update(float elapsedTime);
	void Finalize();

};

//	�o�g���X�e�[�g
template<class T>
class BattleState :public HierarchicalState<T>
{
public:
	BattleState(T* owner) :HierarchicalState<T>(owner) {}
	~BattleState();

	void Initialize();
	void Update(float elapsedTime);
	void Finalize();

};

//	���V�[�u�X�e�[�g
//	���^AI���烁�b�Z�[�W����M�����Ƃ��ɌĂ΂��X�e�[�g
template<class T>
class RecieveState :public HierarchicalState<Enemy>
{
public:
	RecieveState(T* owner):HierarchicalState<Enemy>(owner){}
	~RecieveState();

	void Initialize();
	void Update(float elapsedTime);
	void Finalize();

};

//	�p�j�X�e�[�g�I�u�W�F�N�g
template<class T>
class WanderState : public State<T>
{
public:
	WanderState(T* owner) :State<T>(owner) {};
	~WanderState() {}

	void Initizalize()override;
	void Update(float elapsedTime)override;
	void Finalize()override;

};

//	�ҋ@�X�e�[�g�I�u�W�F�N�g
template<class T>
class IdleState : public State<T>
{
public:
	IdleState(T* owner) :State<T>(owner) {};
	~IdleState() {}
	
	void Initizalize()override;
	void Update(float elapsedTime)override;
	void Finalize()override;

};

//	�ǐՃX�e�[�g�I�u�W�F�N�g
template<class T>
class PursuitState : public State<T>
{
public:
	PursuitState(T* owner) :State<T>(owner) {};
	~PursuitState() {}

	void Initialize()override;
	void Update(float elapsedTime)override;
	void Finalize()override;

};

//	�U���X�e�[�g�I�u�W�F�N�g
template<class T>
class AttackState : public State<T>
{
public:
	AttackState(T* owner) :State<T>(owner) {};
	~AttackState() {}
	
	void Initialize()override;
	void Update(float elapsedTime)override;
	void Finalize()override;

};

//	�R�[���h�X�e�[�g
//	���̃G�l�~�[����Ă΂ꂽ�Ƃ��̃X�e�[�g
template<class T>
class CalledState :public State<T>
{
public:
	CalledState(T* owner) :State<T>(owner) {};
	~CalledState() {}
	
	void Initialize()override;
	void Update(float elapsedTime)override;
	void Finalize()override;

};

//	�퓬�ҋ@�X�e�[�g�I�u�W�F�N�g
template<class T>
class StandbyState :public State<T>
{
public:
	StandbyState(T* owner) :State<T>(owner) {};
	~StandbyState(){}

	void Initialize()override;
	void Update(float elapsedTime)override;
	void Finalize()override;

};
#endif