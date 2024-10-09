#pragma once

#include <memory>

#include "Enemy.h"
#include "../Nova/Resources/Effect.h"
#include "BulletManager.h"
#include "../Nova/AI/StateMachine.h"
#include "../Nova/Audio/Audio.h"

class Drone :public Enemy
{
public:
	enum class StateType	//	�X�e�[�g�̎��
	{
		Idle = 0,			//	�ҋ@
		Search, 			//	�T�[�`
		Move,				//	�ړ�
		Attack,				//	�U��
		Avoidance,			//	���
		Max,				//	�X�e�[�g�ő吔
	};

private:
	//enum class AttackType	//	�U���̎��
	//{
	//	Short = 0,
	//	Long,
	//	Max,
	//};

	enum class Audio3D		//	3D�I�[�f�B�I�Ŗ炵������
	{
		Shot,				//	���ˉ�
		Destroy,			//	�j��
		Max,
	};

public:
	Drone();
	~Drone()override;

	void Initialize()	override;
	void Update(const float& elapsedTime)		override;
	void Render()		override;

	bool RayVsVertical(const float& elapsedTime)override;
	bool RayVsHorizontal(const float& elapsedTime)override;
	void Move(const float& elapsedTime)override {}
	void Attack();
	void LaunchBullet();					//	�e�ې�������
	void Turn(const float& elpasedTime);	//	���񏈗�
	void Destroy()override;					//	�j������

	void UpdateEmitter();	//	�G�~�b�^�[�X�V
	void UpdateAudioSource(const float& elapsedTime);	//	�I�[�f�B�I�\�[�X�X�V

	const int GetMaxHp()const { return MAX_HP; }		//	�ő�HP�擾

	void ChangeState(StateType state) { stateMachine_->ChangeState(static_cast<int>(state)); }	//	�X�e�[�g�J��
	StateMachine<State<Drone>>* GetStateMachine() { return stateMachine_.get(); }	//	�X�e�[�g�}�V���擾

	void DrawDebugPrimitive()override;			//	�f�o�b�O�v���~�e�B�u�`��
	void DrawDebug()	override;				//	�f�o�b�O�`��

private:
	std::shared_ptr <Effect>		effectResource_;							//	�G�t�F�N�g
	float	effectScale_ = 1.0f;
	
	float	launchTimer_ = 1.3f;	//	���̋��𔭎˂���܂ł̃^�C�}�[
	
	AudioSource3D*	sources_[static_cast<int>(Audio3D::Max)] = { nullptr };		//	�I�[�f�B�I�\�[�X
	SoundEmitter	emitter_[static_cast<int>(Audio3D::Max)] = {};				//	�G�~�b�^�[

	std::unique_ptr<StateMachine<State<Drone>>>	stateMachine_ = nullptr;		//	�X�e�[�g�}�V��

private:	//	�f�o�b�O�p�̕ϐ�
	bool	bulletLaunch_	= true;		//	�e�𔭎˂��邩�ǂ���()
	static const int MAX_HP = 3;		//	�ő�HP

};

