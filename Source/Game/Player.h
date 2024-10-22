#pragma once

#include <memory>

#include "Character.h"
#include "../Nova/Resources/Effect.h"
#include "../Nova/AI/StateMachine.h"
#include "../Nova/Audio/Audio3DSystem.h"
#include "../Nova/Input/Input.h"

class Player :public Character
{
public:
	//	�A�j���[�V�������
	enum class AnimationType
	{
		Idle = 0,		//	�ҋ@
		IdleCombat,		//	�ҋ@(�\�����)
		Walk,			//	����
		RunStart,		//	����n��
		Run,			//	������
		RunEnd,			//	����I���
		JumpVertical,	//	�����W�����v	
		JumpFront,		//	�O�W�����v
		JumpRight,		//	�E�W�����v
		JumpBack,		//	���W�����v
		JumpLeft,		//	���W�����v
		DoubleJamp,		//	2�i�W�����v�̂Q�i��
		Combo0_1,		//	�R���{1_1
		Combo0_2,		//	�R���{1_2
		Combo0_3,		//	�R���{1_3
		Combo0_4,		//	�R���{1_4
		DodgeFront,		//	�O���
		DodgeRight,		//	�E���
		DodgeBack,		//	�����
		DodgeLeft,		//	�����
		DodgeAirFront,	//	�󒆑O���
		DodgeAirRight,	//	�󒆉E���
		DodgeAirBack,	//	�󒆌����
		DodgeAirLeft,	//	�󒆍����

		Max,			//	�A�j���[�V�����ő吔
	};

	//	�X�e�[�g�̎��
	enum class StateType
	{
		Idle = 0, 		//	�ҋ@
		Move,			//	�ړ�
		Attack,			//	�U��
		ComboOne1,		//	�R���{0_1
		ComboOne2,		//	�R���{0_2
		ComboOne3,		//	�R���{0_3
		ComboOne4,		//	�R���{0_4
		Dodge,		//	���
		Max,			//	�X�e�[�g�ő吔
	};

public:
	Player();
	~Player()override {}

	static Player& Instance();

	void Initialize()override;
	void Update(const float& elapsedTime)override;
	void Render()override;
	
	//	�f�o�b�O
	void DrawDebug()override;	//	ImGui�`��
	void DrawDebugPrimitive();	//	�f�o�b�O�v���~�e�B�u�`��
	void DrawStateStr();		//	���݂̃X�e�[�g�`��
	void DrawDummyRay();

	bool InputMove(const float& elapsedTime);		//	�ړ����͏���
	void ChangeState(StateType state) { stateMachine_->ChangeState(static_cast<int>(state)); }	//	�X�e�[�g�J��
	void PlayEffect();

	const bool GetCombo0ButtonDown() { return Input::Instance().GetGamePad().GetButtonDown()& GamePad::BTN_B; }

	//	����
	bool RayVsVertical(const float& elapsedTime)override;		//	�X�e�[�W�Ƃ̓����蔻��(��������)
	bool RayVsHorizontal(const float& elapsedTime)override;		//	�X�e�[�W�Ƃ̓����蔻��(��������)	
	bool PlayerVsEnemies(const float& elapsedTime);				//	������������
	bool JointVsEnemiesAndBullet(const float& elapsedTime, const std::string& meshName, const std::string& boneName, const float& jointRadius);
	bool JointVsEnemies(const float& elapsedTime, const DirectX::XMFLOAT3& jointPos, const float jointRadius);	//	�W���C���g�ƓG�̓����蔻��
	bool JointVsBullet(const float& elapsedTime, const DirectX::XMFLOAT3& jointPos, const float jointRadius);	//	�W���C���g�ƒe�ۂ̓����蔻��
	bool DummyRay(const float& elapsedTime);	//	���C�L���X�g�ł����Ə�񂪎��Ă��邩

	void PlayAnimation(AnimationType index, const bool& loop = false, const float& speed = 1.0f, const float& blendTime = 1.0f, const float& startFrame = 0.0f);
	//void PlayBlendAnimation(AnimationType index, bool loop, float speed = 1.0f);
	//void PlayBlendAnimation(AnimationType index1, AnimationType index2, bool loop, float speed = 1.0f) { GameObject::PlayBlendAnimation(static_cast<int>(index1), static_cast<int>(index2), loop, speed); }
	
	void UpdateListener();	//	���X�i�[���X�V

	void SetIsPose(bool isPose)							{ isPose_ = isPose; }
	void SetIsHitEnemy(bool isHitEnemy)					{ isHitEnemy_ = isHitEnemy; }
	void SetEffectScale(float scale)					{ effectScale_ = scale; }
	void SetPlayEffectFlag(bool playEffect)				{ playEffectFlag_ = playEffect; }
	void SetEffectPos(const DirectX::XMFLOAT3& pos)		{ effectPos_ = pos; }

	const int	GetMaxHp()		const	{ return MAX_HP; }
	bool		GetPose()		const	{ return isPose_; }
	bool		IsHItEnemy()	const	{ return isHitEnemy_; }
	bool		IsPlayEffect()	const	{ return playEffectFlag_; }
	int									GetCurrentAnimNum();			//	���ݍĐ����̃A�j���[�V�����ԍ��擾
	AnimationType						GetCurrentAnimType();			//	���ݍĐ����̃A�j���[�V�����^�C�v�擾
	float						const	GetCurrentAnimationSeconds();	//	���݂̃A�j���[�V�����Đ����Ԏ擾
	DirectX::XMFLOAT3					GetMoveVec()const;				//	�X�e�B�b�N���͒l����ړ��x�N�g�����擾
	StateMachine<State<Player>>*		GetStateMachine()	{ return stateMachine_.get(); }		//	�X�e�[�g�}�V���擾
	SoundListener						GetListener()const	{ return listener_; }				//	���X�i�[�擾

private:
	std::shared_ptr <Effect>		effectResource_;										//	�G�t�F�N�g
	float							effectScale_ = 5.0f;									//	�G�t�F�N�g�X�P�[��
	DirectX::XMFLOAT3				effectPos_ = {};										//	�G�t�F�N�g�Đ��ʒu
	bool							playEffectFlag_ = false;								//	�G�t�F�N�g�`��t���O
	//AnimationType					currentAnimNum_;										//	���݂̃A�j���[�V�����ԍ�
	float							turnSpeed_ = DirectX::XMConvertToRadians(720);			//	���񑬓x
	std::unique_ptr<StateMachine<State<Player>>>	stateMachine_ = nullptr;				//	�X�e�[�g�}�V��
	static constexpr int			MAX_HP = 100;

	bool							isPose_ = false;										//	�|�[�Y���v���C���[�̑�����󂯕t���Ȃ�
	bool							isHitEnemy_ = false;									//	�G�l�~�[�Ɠ������Ă��邩(�����o���p)

	//	�I�[�f�B�I
	SoundListener listener_ = {};	//	���X�i�[

	//	ImGui�p
	bool				isCollisionStage_	= true;
	bool				isHitStage_			= false;
	float				gravity_			= -5.0f;
	float				rayPosRadius_		= 0.3f;
	DirectX::XMFLOAT3	coneDirection_		= { 0,1,0 };	//	�~���̕���
	bool				isAddGravity_		= false;		//	�d�͉��Z�t���O

	//	DummyRay
	float debugOffset_ = height_ / 2;
	bool				isDummyReset_		= false;
	bool				isDummyHit_			= false;		//	���Ă��Ă��邩�ǂ���
	float				dummyRayLimit_		= 100.0f;		//	���C�̒���
	DirectX::XMFLOAT3	hitPosition_		= {};			//	���������ʒu
	DirectX::XMFLOAT3	hitNormal_			= {};			//	�@���̕���
	std::string			hitMesh_			= {};			//	���b�V����
	std::string			hitMaterial_		= {};			//	�}�e���A����

};