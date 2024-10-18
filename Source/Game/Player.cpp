#include "Player.h"

#include "../Nova/Graphics/Graphics.h"
#include "../Nova/Input/GamePad.h"
#include "../Nova/Input/Input.h"
#include "../Nova/Graphics/Camera.h"
#include "../Nova/Core/Framework.h"
#include "../Nova/Others/MathHelper.h"
#include "../Nova/Collision/Collision.h"
#include "PlayerState.h"
#include "Stage.h"
#include "EnemyManager.h"

static Player* instance = nullptr;

// �C���X�^���X�擾
Player& Player::Instance()
{
	return *instance;
}

//	�R���X�g���N�^
Player::Player()
	:Character("./Resources/Model/free-mixamo-retextured-model/source/model5.glb", "")
{
	//	�C���X�^���X�ݒ�
	instance = this;

	//	�X�e�[�g�Z�b�g(Player::StateType�̏��ƍ��킹��)
	stateMachine_.reset(new StateMachine<State<Player>>());
	stateMachine_->RegisterState(new PlayerState::IdleState(this));			//	�ҋ@
	stateMachine_->RegisterState(new PlayerState::MoveState(this));			//	�ړ�
	stateMachine_->RegisterState(new PlayerState::AttackState(this));		//	�U��
	stateMachine_->RegisterState(new PlayerState::AvoidanceState(this));	//	���

	stateMachine_->SetState(static_cast<int>(StateType::Idle));				//	�����X�e�[�g�Z�b�g
	PlayAnimation(Player::AnimationType::ANIM_IDLE, true, 1.0f, 0.0f);

	//	���X�i�[���Z�b�g
	listener_.innerRadius_ = 0.7f;
	listener_.outerRadius_ = 1.67f;
	listener_.filterParam_ = 0.8f;

}

//	������
void Player::Initialize()
{
	//	�G�t�F�N�g�ǂݍ���
	effectResource_ = ResourceManager::Instance().LoadEffectResource("./Resources/Effect/HitEff.efk");

	//	�G�t�F�N�g�X�P�[���ݒ�
	effectScale_ = 5.0f;

	GetTransform()->SetPosition({ 0.0f, 5.0f, 0.0f });

	//float scale = 2.0f;
	GetTransform()->SetScaleFactor(0.0225f);
	radius_ = 0.7f;
	height_ = 4.4f;

	moveSpeed_ = 2.0f;
	//moveSpeed_ = 25.0f;

	hp_ = MAX_HP;

}

//	�X�V����
void Player::Update(const float& elapsedTime)
{
	if (!isPose_)	//	�|�[�Y������Ȃ��Ȃ�
	{
		//	�X�e�[�g���Ƃ̍X�V����
		stateMachine_->Update(elapsedTime);

		//	�G�t�F�N�g�Đ��m�F�p
#if _DEBUG
		//GamePad& gamePad = Input::Instance().GetGamePad();
		//if (gamePad.GetButtonDown() & GamePad::BTN_A)	//	Z�L�[����(�G�t�F�N�g�m�F�p)
		//{
		//	DirectX::XMFLOAT3 pos = this->GetTransform()->GetPosition();
		//	pos.y += height_;
		//	effectResource_->Play(pos, effectScale_);
		//}
#endif
		//	�G�Ƃ̓����蔻��
		PlayerVsEnemy(elapsedTime);

	
		if (!isHitStage_ && isAddGravity_)
		{
			//	�K���ɏd�͏���
			AddVelocityY(gravity_, elapsedTime);
			//GetTransform()->SetPositionY(GetTransform()->GetPositionY() - gravity_ * elapsedTime);
			//Move(elapsedTime);	//	inputMove�ɂ�����
		}
		//	�X�e�[�W�Ƃ̓����蔻��
		if (isCollisionStage_)
		{
			//isHitStage_ = RayVsVertical(elapsedTime);	//	��������(�n��)
			if (!RayVsVertical(elapsedTime))
			{
				GetTransform()->AddPositionY(velocity_.y * elapsedTime);
			}
			RayVsHorizontal(elapsedTime);	//	��������(��)
		}

		//Move(elapsedTime);	//	inputMove�ɂ�����

	}

	//	�A�j���[�V�����X�V����
	UpdateAnimation(elapsedTime);
	
	//DummyRay(elapsedTime);

	//	���X�i�[�X�V
	UpdateListener();

}

//	���X�i�[���X�V
void Player::UpdateListener()
{
	//	�������ɕω�����l
	DirectX::XMFLOAT3 position = GetTransform()->GetPosition();
	DirectX::XMFLOAT3 scale = GetTransform()->GetScale();

	//listener_.position = { position.x, position.y + (scale.y / 2.0f), position.z };
	listener_.position_ = { position.x, position.y + height_ / 2, position.z };
	listener_.frontVec_ = Camera::Instance().GetFront();
	listener_.velocity_ = GetMoveVec();
	listener_.rightVec_ = Camera::Instance().GetRight();

}

//	�v���C���[�ƓG�̓����蔻��i�������������j
bool Player::PlayerVsEnemy(const float& elapsedTime)
{
	DirectX::XMFLOAT3 pos = GetTransform()->GetPosition();
	pos.y += height_ / 2.0f;
	EnemyManager& enemyManager = EnemyManager::Instance();
	DirectX::XMFLOAT3 outPosition = {};

	isHitEnemy_ = false;

	for (Enemy* enemy : enemyManager.GetEnemies())
	{
		DirectX::XMFLOAT3 ePos = enemy->GetTransform()->GetPosition();
		float eRadius = enemy->GetRadius() - 5.0f;
		float eHeight = enemy->GetHeight();
		DirectX::XMFLOAT3 ePosOffset = { 0.0f,-eHeight / 2.0f,0.0f };

		//	�~���Ɖ~���œ����蔻��
		if (Collision::IntersectCylinderVsCyliner(pos, radius_, height_, ePos + ePosOffset, eRadius, eHeight, outPosition, true))
		{
			isHitEnemy_ = true;
			GetTransform()->SetPositionX(outPosition.x);
			GetTransform()->SetPositionZ(outPosition.z);
		}

	}
	return isHitEnemy_;
}

//	�ړ����͏���
bool Player::InputMove(const float& elapsedTime)
{
	//	�i�s�x�N�g���擾
	moveVec_ = GetMoveVec();

	//	�ړ�����
	UpdateVelocity(elapsedTime);
	Move(elapsedTime);

	//	���񏈗�
	Turn(elapsedTime, moveVec_.x, moveVec_.z, turnSpeed_);

	//	�i�s�x�N�g�����[���x�N�g���łȂ��ꍇ�͓��͂��ꂽ
	//	(�[�����傫����Γ��͂��ꂽ)
	float moveVecLength = sqrtf(moveVec_.x * moveVec_.x + moveVec_.z * moveVec_.z);
	return (moveVecLength > 0);

}

//	�U���X�e�[�g�֑J��
void Player::TransitionAttack()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	//	X�L�[����������U���X�e�[�g�֑J��
	if (gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		stateMachine_->ChangeState(static_cast<int>(StateType::Attack));
	}

}

////	�u�����h�A�j���[�V����
//void Player::PlayBlendAnimation(AnimationType index, bool loop, float speed)
//{
//	GameObject::PlayBlendAnimation(GetCurrentBlendAnimationIndex(), static_cast<int>(index), loop, speed);
//}

////	�X�e�[�W�Ƃ̓����蔻��	��������
//bool Player::RayVsVertical(const float& elapsedTime)
//{
//	DirectX::XMFLOAT3 rayStartPos = GetTransform()->GetPosition();	//	���C�̎n�_
//	float centerOffset = height_ / 2.0f;							//	�n�_���v���C���[�̒��S�֕␳
//	rayStartPos.y += centerOffset;
//	DirectX::XMFLOAT3	rayDirection = { 0,-1,0 };					//	���C�̕���(�^��)
//	DirectX::XMFLOAT4X4 transform	= {};							//	�X�e�[�W�̃��[���h�ϊ��s��
//	DirectX::XMStoreFloat4x4(&transform, Stage::Instance().GetTransform()->CalcWorld());
//
//	float	rayLimit = centerOffset - velocity_.y * elapsedTime;	//	���C�̒���
//	//float	rayLimit = 9.0f;	//	���C�̒���
//	bool	skipIf = false;
//
//	//	���C�̊J�n�_�`��
//#if 1
//	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
//	debugRenderer->DrawSphere(rayStartPos, rayPosRadius_, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));	//	��
//#endif
//
//	//	���C�̏I�_�`��
//#if 1
//	DirectX::XMFLOAT3 rayEndPos = {};
//	DirectX::XMVECTOR RayEndPos = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&rayStartPos), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&rayDirection), rayLimit));
//	DirectX::XMStoreFloat3(&rayEndPos, RayEndPos);
//	debugRenderer = Graphics::Instance().GetDebugRenderer();
//	debugRenderer->DrawSphere(rayEndPos, rayPosRadius_, DirectX::XMFLOAT4(0.0f, 1.0f, 0.6f, 1.0f));		//	��
//#endif
//
//	DirectX::XMFLOAT3	intersectionPosition	= {};			//	���������ʒu
//	DirectX::XMFLOAT3	intersectionNormal		= {};			//	�@���̕���
//	std::string			intersectionMesh		= {};			//	���b�V����
//	std::string			intersectionMaterial	= {};			//	�}�e���A����
//
//	//	�����蔻�菈��
//	bool isHit = false;
//	isHit = Stage::Instance().Collision(rayStartPos, rayDirection, transform, intersectionPosition, intersectionNormal, intersectionMesh, intersectionMaterial, rayLimit, skipIf);
//
//	//	�n�ʂɃ��C������������
//	if (isHit)
//	{
//		hitPosition_ = intersectionPosition;
//		hitNormal_ = intersectionNormal;
//#if 0 
//		DirectX::XMFLOAT3 pos = {};
//		DirectX::XMVECTOR intersectPos = DirectX::XMLoadFloat3(&intersectionPosition);
//		DirectX::XMVECTOR pushVec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&intersectionNormal)), rayLimit);
//		
//		DirectX::XMStoreFloat3(&pos, DirectX::XMVectorSubtract(intersectPos, pushVec));
//
//		GetTransform()->SetPosition(pos);
//#else
//		DirectX::XMVECTOR	IntersectionPos = DirectX::XMLoadFloat3(&intersectionPosition);			//	���C�����������ʒu
//		DirectX::XMFLOAT3	pos				= GetTransform()->GetPosition();						//	�v���C���[�̌��݂̈ʒu(����)
//		DirectX::XMVECTOR	Pos				= DirectX::XMLoadFloat3(&pos);							//	�v���C���[�̌��݂̈ʒu�x�N�g��
//		DirectX::XMVECTOR	Push			= DirectX::XMVectorSubtract(IntersectionPos, Pos);		//	���������ʒu����v���C���[�܂ł̃x�N�g��
//		
//		DirectX::XMVectorSetX(Push, 0.0f);
//		DirectX::XMVectorSetZ(Push, 0.0f);
//
//		float				pushLength		= DirectX::XMVectorGetY(DirectX::XMVector3Length(Push));
//		DirectX::XMFLOAT3	push = {};
//		DirectX::XMStoreFloat3(&push, Push);
//		if (pushLength > 0.0001f)
//		{
//			//GetTransform()->SetPosition(intersectionPosition);
//			//GetTransform()->AddPosition(push);
//			//GetTransform()->AddPositionY(push.y);
//		}
//
//		DirectX::XMFLOAT3 posDebug = GetTransform()->GetPosition();
//
//		/*DirectX::XMVECTOR Velocity = DirectX::XMLoadFloat3(&velocity_);
//		Velocity = DirectX::XMVectorAdd(Velocity, Push);
//		DirectX::XMStoreFloat3(&velocity_, Velocity);*/
//
//		velocity_.y = 0.0f;
//#endif
//
//		//	���C�����������ʒu
//#if 1
//		DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
//		debugRenderer->DrawSphere(intersectionPosition, rayPosRadius_ + 1.0f, DirectX::XMFLOAT4(1, 1, 1, 1));	//	��
//#endif
//		
//	}
//
//	return isHit;
//}

//	COLLISION_MESH_2
//	�X�e�[�W�Ƃ̓����蔻��	��������
bool Player::RayVsVertical(const float& elapsedTime)
{
	DirectX::XMFLOAT3 rayStartPos;									//	���C�̎n�_
	DirectX::XMFLOAT3 rayDirection;									//	���C�̕���
	float liftup = height_ / 2.0f;									//	���C�̎n�_���v���C���[�̒��S�֎����グ��
	DirectX::XMVECTOR RayPos = DirectX::XMLoadFloat3(&GetTransform()->GetPosition());							//	���C�̎n�_
	DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));	//	���C�̕���
	DirectX::XMVECTOR Liftup = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.0f, liftup, 0.0f, 1.0f));		//	LIFTUP
	DirectX::XMStoreFloat3(&rayStartPos, DirectX::XMVectorAdd(RayPos, Liftup));
	DirectX::XMStoreFloat3(&rayDirection, Direction);

	DirectX::XMFLOAT3 playerPos = GetTransform()->GetPosition();	//	�v���C���[�̈ʒu(��������_)

	DirectX::XMFLOAT4X4 transform = {};								//	�X�e�[�W�̃��[���h�ϊ��s��
	DirectX::XMStoreFloat4x4(&transform, Stage::Instance().GetTransform()->CalcWorld());

	//	���C�̊J�n�_�`��
#if 1
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
	debugRenderer->DrawSphere(rayStartPos, rayPosRadius_, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));	//	��
#endif

	//	Collision()�̌��ʊi�[�p
	DirectX::XMFLOAT3	intersectionPosition = {};			//	���������ʒu
	DirectX::XMFLOAT3	intersectionNormal = {};			//	�@���̕���
	std::string			intersectionMesh = {};				//	���b�V����
	std::string			intersectionMaterial = {};			//	�}�e���A����

	//	�����蔻�菈��
	bool isHit = false;
	//	���C�ƒn�ʂ��������Ă�����
	if (Stage::Instance().Collision(rayStartPos, rayDirection, transform, intersectionPosition, intersectionNormal, intersectionMesh, intersectionMaterial))
	{
		float d0 = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&playerPos) - DirectX::XMLoadFloat3(&rayStartPos)));
		float d1 = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&intersectionPosition) - DirectX::XMLoadFloat3(&rayStartPos)));

		//	�v���C���[�ƒn�ʂ��������Ă�����
		if (d0 + radius_ > d1)
		{
			//	�v���C���[�̈ʒu��␳
			float d = d0 - d1;
			playerPos.x -= d * rayDirection.x;
			playerPos.y -= d * rayDirection.y;
			playerPos.z -= d * rayDirection.z;

			GetTransform()->SetPosition(playerPos);

			// Reflection
			DirectX::XMStoreFloat3(&velocity_, DirectX::XMVector3Reflect(DirectX::XMLoadFloat3(&velocity_), DirectX::XMLoadFloat3(&intersectionNormal)));

			isHit = true;

			//	�f�o�b�O�`��
			//	���C�����������ʒu
#if 1
			DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
			debugRenderer->DrawSphere(intersectionPosition, rayPosRadius_ + 1.0f, DirectX::XMFLOAT4(1, 1, 1, 1));	//	��
#endif

		}

	}

	return isHit;
}

//	�X�e�[�W�Ƃ̓����蔻��	��������
//bool Player::RayVsHorizontal(const float& elapsedTime)
//{
//	//	�������͌v�Z
//	float velocityLengthXZ = sqrtf(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
//	if (velocityLengthXZ > 0.0f)
//	{
//		//	�����ړ��l
//		float mx = velocity_.x * elapsedTime;
//		float mz = velocity_.z * elapsedTime;
//
//		//	���C�̊J�n�ʒu�ƕ���
//		DirectX::XMFLOAT3	position		= GetTransform()->GetPosition();
//		DirectX::XMFLOAT3	rayStartPos			= position;
//		DirectX::XMFLOAT3	radiusOffset	= {};
//		DirectX::XMVECTOR	Move			= DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&GetMoveVec()));
//		Move = DirectX::XMVectorSetY(Move, 0.0f);
//		DirectX::XMVECTOR	RadiusOffset	= DirectX::XMVectorScale(Move, radius_);				//	�ړ������̌����ɔ��a�𑫂�
//		DirectX::XMStoreFloat3(&radiusOffset, RadiusOffset);
//		float				heightOffset	= height_ / 2.0f;										//	���C�̎n�_���v���C���[�̒��S�֕␳
//		rayStartPos.y += heightOffset;
//
//		DirectX::XMFLOAT3	rayEnd			= { rayStartPos.x + mx  ,rayStartPos.y , rayStartPos.z + mz };	//	���C�̏I���n�_
//		DirectX::XMVECTOR	RayPos			= DirectX::XMLoadFloat3(&rayStartPos);				//	���C�̊J�n�_
//		DirectX::XMVECTOR	RayEnd			= DirectX::XMLoadFloat3(&rayEnd);
//		RayEnd = DirectX::XMVectorAdd(RayEnd, Move);
//		float				rayLimit		= DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(RayEnd, RayPos))) + radius_;
//		DirectX::XMFLOAT3	rayDirection	= {};
//		DirectX::XMVECTOR	RayDirection	= DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(RayEnd, RayPos));
//		DirectX::XMStoreFloat3(&rayDirection, RayDirection);
//		//	�����ɔ�΂�
//		rayDirection.y = 0;
//
//		//	�X�e�[�W�̃��[���h�ϊ��s��
//		DirectX::XMFLOAT4X4 transform = {};							
//		DirectX::XMStoreFloat4x4(&transform, Stage::Instance().GetTransform()->CalcWorld());
//
//		//	���C�̊J�n�_�`��
//#if 1
//		DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
//		debugRenderer->DrawSphere(rayStartPos, rayPosRadius_, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));	//	��
//#endif
//
//		//	���C�̏I�_�`��
//#if 0
//		DirectX::XMFLOAT3 rayEndPos = {};
//		DirectX::XMVECTOR RayEndPos = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&rayStartPos), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&rayDirection), rayLimit));
//		DirectX::XMStoreFloat3(&rayEndPos, RayEndPos);
//		debugRenderer = Graphics::Instance().GetDebugRenderer();
//		debugRenderer->DrawSphere(rayEndPos, rayPosRadius_, DirectX::XMFLOAT4(0.0f, 1.0f, 0.6f, 1.0f));	//	��
//#endif
//
//		//	���C�L���X�g�ɂ��ǔ���
//		DirectX::XMFLOAT3	intersectionPosition	= {};			//	���������ʒu
//		DirectX::XMFLOAT3	intersectionNormal		= {};			//	�@���̕���
//		std::string			intersectionMesh		= {};			//	���b�V����
//		std::string			intersectionMaterial	= {};			//	�}�e���A����
//		bool skipIf = false;
//
//		if (Stage::Instance().Collision(rayStartPos, rayDirection, transform, intersectionPosition, intersectionNormal, intersectionMesh, intersectionMaterial, rayLimit, skipIf))
//		{
//			//	�ǂ܂ł̃x�N�g��
//			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&intersectionPosition);
//			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&rayEnd);
//			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
//
//			//	�ǂ̖@��
//			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&intersectionNormal);
//
//			//	���˃x�N�g����@���Ɏˉe
//			float projectionL = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal)) * 1.5f;
//
//			//	�␳�ʒu�̌v�Z
//			DirectX::XMVECTOR CollectPosition = DirectX::XMVectorAdd(
//				DirectX::XMVectorScale(Normal, projectionL), End);
//			DirectX::XMFLOAT3 collectPosition{};
//			DirectX::XMStoreFloat3(&collectPosition, CollectPosition);
//
//			//	�ǂ����̈ʒu���߂肱��ł��Ȃ����`�F�b�N
//			intersectionPosition	= {};			//	���������ʒu
//			intersectionNormal		= {};			//	�@���̕���
//			intersectionMesh		= {};			//	���b�V����
//			intersectionMaterial	= {};			//	�}�e���A����
//			DirectX::XMStoreFloat3(&rayEnd, CollectPosition);
//			rayLimit = DirectX::XMVectorGetX(DirectX::XMVectorSubtract(RayEnd, RayPos));
//			rayDirection = {};
//			RayDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(RayEnd, RayPos));
//			DirectX::XMStoreFloat3(&rayDirection, RayDirection);
//
//			if (Stage::Instance().Collision(rayStartPos, rayDirection, transform, intersectionPosition, intersectionNormal, intersectionMesh, intersectionMaterial, rayLimit, skipIf))
//			{
//				CollectPosition = DirectX::XMLoadFloat3(&intersectionPosition);
//			}
//			else
//			{
//				position.x = collectPosition.x;
//				position.z = collectPosition.z;
//			}
//
//			//	���C�����������ʒu
//#if 1
//			DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
//			debugRenderer->DrawSphere(intersectionPosition, rayPosRadius_, DirectX::XMFLOAT4(1, 1, 1, 1));	//	��
//#endif
//
//		}
//		else
//		{
//			////	�ړ�
//			//position.x += mx;
//			//position.z += mz;
//		}
//
//		GetTransform()->SetPosition(position);
//	}
//
//	return false;
//}

bool Player::RayVsHorizontal(const float& elapsedTime)
{
	DirectX::XMFLOAT3 rayStartPos;									//	���C�̎n�_
	DirectX::XMFLOAT3 rayDirection;									//	���C�̕���
	float liftup = height_ / 2.0f;									//	���C�̎n�_���v���C���[�̒��S�֎����グ��
	DirectX::XMVECTOR RayPos = DirectX::XMLoadFloat3(&GetTransform()->GetPosition());							//	���C�̎n�_
	DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(DirectX::XMVectorSet(velocity_.x, 0.0f, velocity_.z, 0.0f));	//	���C�̕���
	DirectX::XMVECTOR Liftup = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.0f, liftup, 0.0f, 1.0f));		//	LIFTUP
#if 1
	DirectX::XMStoreFloat3(&rayStartPos, DirectX::XMVectorAdd(RayPos, Liftup));
#else
	float stepBack = 1.0f;
	DirectX::XMStoreFloat3(&rayStartPos, DirectX::XMVectorSubtract(RayPos, DirectX::XMVectorScale(Direction, stepBack)));
#endif
	DirectX::XMStoreFloat3(&rayDirection, Direction);

	DirectX::XMFLOAT3 playerPos = GetTransform()->GetPosition();	//	�v���C���[�̈ʒu(��������_)

	DirectX::XMFLOAT4X4 transform = {};								//	�X�e�[�W�̃��[���h�ϊ��s��
	DirectX::XMStoreFloat4x4(&transform, Stage::Instance().GetTransform()->CalcWorld());

	//	���C�̊J�n�_�`��
#if 1
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
	debugRenderer->DrawSphere(rayStartPos, rayPosRadius_, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));	//	��
#endif

	//	Collision()�̌��ʊi�[�p
	DirectX::XMFLOAT3	intersectionPosition = {};			//	���������ʒu
	DirectX::XMFLOAT3	intersectionNormal = {};			//	�@���̕���
	std::string			intersectionMesh = {};				//	���b�V����
	std::string			intersectionMaterial = {};			//	�}�e���A����

	//	�����蔻�菈��
	bool isHit = false;
	//	���C�ƒn�ʂ��������Ă�����
	if (Stage::Instance().Collision(rayStartPos, rayDirection, transform, intersectionPosition, intersectionNormal, intersectionMesh, intersectionMaterial))
	{
		float d0 = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&playerPos) - DirectX::XMLoadFloat3(&rayStartPos)));
		float d1 = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&intersectionPosition) - DirectX::XMLoadFloat3(&rayStartPos)));

		float rayOffset = 0.5f;	//	���C�̒������������₷

		//	�v���C���[�ƒn�ʂ��������Ă�����
		if (d0 + radius_ + rayOffset > d1)
		{
			//	�v���C���[�̈ʒu��␳
			float d = d0 - d1;
			playerPos.x -= d * rayDirection.x;
			playerPos.y -= d * rayDirection.y;
			playerPos.z -= d * rayDirection.z;

			GetTransform()->SetPosition(playerPos);

			// Reflection
			DirectX::XMStoreFloat3(&velocity_, DirectX::XMVector3Reflect(DirectX::XMLoadFloat3(&velocity_), DirectX::XMLoadFloat3(&intersectionNormal)));

			isHit = true;

			//	�f�o�b�O�`��
			//	���C�����������ʒu
#if 1
			DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
			debugRenderer->DrawSphere(intersectionPosition, rayPosRadius_ + 1.0f, DirectX::XMFLOAT4(1, 1, 1, 1));	//	��
#endif

		}

	}

	return isHit;
}

//	�G�t�F�N�g�Đ�
void Player::PlayEffect()
{
	//	�G�t�F�N�g�`��
	effectResource_->Play(effectPos_, effectScale_);

	//	�G�t�F�N�g�`��t���O���Z�b�g
	playEffectFlag_ = false;
}

bool Player::DummyRay(const float& elapsedTime)
{
	//	�E��̃��[���h���W�擾
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, GetTransform()->CalcWorld());	//	�v���C���[�̃��[���h�s��
	DirectX::XMFLOAT3 leftHandPos = GetJointPosition("Ch44", "mixamorig:RightHandMiddle1", world);

	//	�����蔻��p�̔��a�Z�b�g
	constexpr float leftHandRadius = 50.0f;

	//	�Փ˔���p�̃f�o�b�O����`��
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
	debugRenderer->DrawSphere(leftHandPos, leftHandRadius, DirectX::XMFLOAT4(1, 1, 1, 1));

	DirectX::XMFLOAT3	rayPos = leftHandPos;					//	���C�̎n�_
	DirectX::XMFLOAT3	rayDirection = { 0,-1,0 };              //	���C�̕���(�^��)
	DirectX::XMFLOAT4X4 transform = {};							//	�X�e�[�W�̃��[���h�ϊ��s��
	DirectX::XMStoreFloat4x4(&transform, Stage::Instance().GetTransform()->CalcWorld());
	float	rayLimit = dummyRayLimit_;							//	���C�̒���(���̂Ƃ���velocity)
	bool	skipIf = true;										//	

	DirectX::XMFLOAT3 rayEndPos = {};
	DirectX::XMVECTOR RayEndPos = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&rayPos), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&rayDirection), rayLimit));
	DirectX::XMStoreFloat3(&rayEndPos, RayEndPos);
	isDummyHit_ = Stage::Instance().Collision(rayPos, rayDirection, transform, hitPosition_, hitNormal_, hitMaterial_, hitMesh_, rayLimit, skipIf);
	
	//	���C���������Ă��Ȃ�������q�b�g��񃊃Z�b�g
	if (isDummyReset_)
	{
		hitPosition_	= {};
		hitNormal_		= {};
		hitMesh_		= {};
		hitMaterial_	= {};
	}

	return isDummyHit_;
}

//	�A�j���[�V����
void Player::PlayAnimation(AnimationType index, const bool& loop, const float& speed, const float blendTime, const float cutTime)
{
	Character::PlayAnimation(static_cast<int>(index), loop, speed, blendTime, cutTime);
}

//	�X�e�B�b�N���͒l����ړ��x�N�g�����擾
DirectX::XMFLOAT3 Player::GetMoveVec()const
{
	//	���͏����擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//	�J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFoward = camera.GetFront();

	//	�ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���
	//	�J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float Rlength;
	DirectX::XMStoreFloat(&Rlength, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&cameraRight)));
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		//	�P�ʃx�N�g����
		cameraRightX = cameraRight.x / Rlength;
		cameraRightZ = cameraRight.z / Rlength;
	}

	//	�J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float Zlength;
	DirectX::XMStoreFloat(&Zlength, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&cameraFoward)));
	float cameraFrontX = cameraFoward.x;
	float cameraFrontZ = cameraFoward.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		//	�P�ʃx�N�g����
		cameraFrontX = cameraFoward.x / Zlength;
		cameraFrontZ = cameraFoward.z / Zlength;
	}

	//	�X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
	//	�X�e�B�b�N�������͒l���J�����O�����ɔ��f���A
	//	�i�s�x�N�g�����v�Z����
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraFrontX * ay + cameraRightX * ax) * moveSpeed_;
	vec.z = (cameraFrontZ * ay + cameraRightZ * ax) * moveSpeed_;

	//	Y�������ɂ͈ړ����Ȃ�
	vec.y = 0.0f;

	return vec;
}

//	�`�揈��
void Player::Render()
{
	//	�s�N�Z���V�F�[�_�[�Z�b�g
	//Graphics::Instance().GetShader()->CreatePsFromCso(Graphics::Instance().GetDevice(), "./Resources/Shader/DronePS.cso", pixelShader_.ReleaseAndGetAddressOf());
	//this->SetPixelShader(pixelShader_.Get());
	Character::Render();

	//	�G�t�F�N�g�`��
	Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_OFF_ZW_OFF);
	if (playEffectFlag_)PlayEffect();

}

//	���ݍĐ����̃A�j���[�V�����ԍ��擾
int Player::GetCurrentAnimNum()
{
	return Character::GetCurrentAnimNum();
}

//	���ݍĐ����̃A�j���[�V�����^�C�v�擾
Player::AnimationType Player::GetCurrentAnimType()
{
	int currentAnimNum = Character::GetCurrentAnimNum();

	return static_cast<Player::AnimationType>(currentAnimNum);
}

//	�f�o�b�O�`��
void Player::DrawDebug()
{
	//	�X�e�[�g������
	std::string stateStr[static_cast<int>(StateType::Max)] =
	{
		"Idle","Move","Attack","Avoidance"
	};

	//	�X�e�[�W�q�b�g������
	std::string hitStage = "";
	if (isHitStage_)hitStage = "true";
	else hitStage = "false";

	//	�A�j���[�V�����֘A
	//int currentAnimationIndex = GetCurrentBlendAnimationIndex();	//	���݂̃A�j���[�V�����ԍ��擾
	//float weight = GetWeight();										//	weight�l�擾		
	//float blendRate = GetBlendRate();

	if (ImGui::TreeNode(u8"Player�v���C���["))
	{
		ImGui::Checkbox("IsPose", &isPose_);
		ImGui::Checkbox("PlayEffect", &playEffectFlag_);
		ImGui::Checkbox("Invincible", &isInvincible_);
		ImGui::Checkbox("AddGravity", &isAddGravity_);

		Character::DrawDebug();
	
		ImGui::DragFloat("Gravity", &gravity_, 0.1f, 0.0f);												//	�d��
		ImGui::DragFloat("EffectScale", &effectScale_, 0.01f, -FLT_MAX, FLT_MAX);						//	�G�t�F�N�g�X�P�[��
		//ImGui::DragFloat("AnimationSpeed", &animationSpeed_, 0.01f, -FLT_MAX, FLT_MAX);				//	�A�j���[�V�����Đ����x
		//ImGui::DragFloat("AnimationWeight", &weight, 0.005f, 0.0f, 1.0f);								//	�A�j���[�V����weight�l
		//ImGui::DragFloat("BlendRate", &blendRate, 0.005f, 0.0f, 1.0f);								//	�A�j���[�V�����u�����h��
		//ImGui::InputInt("CurrentBlendAnimationIndex", &currentAnimationIndex);						//	���݂̃A�j���[�V�����ԍ�
		ImGui::Text(u8"State�@%s", stateStr[static_cast<int>(stateMachine_->GetStateIndex())].c_str());	//	�X�e�[�g�\��
		ImGui::Checkbox(u8"StageCollision", &isCollisionStage_);										//	�X�e�[�W�Ƃ̓����蔻��I��/�I�t
		ImGui::Text(u8"HitStage %s", hitStage.c_str());													//	�X�e�[�W�Ɠ������Ă��邩
		ImGui::DragFloat("Gravity", &gravity_, 0.01f, -FLT_MAX, FLT_MAX);								//	�d��
		ImGui::DragFloat("MoveSpeed", &moveSpeed_, 0.01f, 0.0f, FLT_MAX);								//	�ړ����鑬��
		
		ImGui::DragFloat("RayPosRadius", &rayPosRadius_);	//	���C�L���X�g�̎n�_�I�_��\�����̔��a
		
		ImGui::DragFloat3("ConeDirection", &coneDirection_.x, 0.01f, -FLT_MAX, FLT_MAX);
		
		DrawDummyRay();

		if (ImGui::TreeNode("3DAudio_Listener"))
		{
			//	���X�i�[���
			ImGui::DragFloat3("Position", &listener_.position_.x);
			ImGui::DragFloat("InnerRadius", &listener_.innerRadius_);
			ImGui::DragFloat("OuterRadius", &listener_.outerRadius_);
			ImGui::DragFloat("FilterParam", &listener_.filterParam_);
			ImGui::DragFloat3("FrontVec", &listener_.frontVec_.x);
			ImGui::DragFloat3("RightVec", &listener_.rightVec_.x);
			ImGui::DragFloat3("Velocity", &listener_.velocity_.x);

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	//	ImGui�ł̕ω��𔽉f������
	//SetWeight(weight);
	//SetBlendRate(blendRate);

}

//	�f�o�b�O�v���~�e�B�u�`��
void Player::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//	�Փ˔���p�̃f�o�b�O�~����`��
	debugRenderer->DrawCylinder(this->GetTransform()->GetPosition(), radius_, height_, DirectX::XMFLOAT4(0, 0, 0, 1));

	//	�~����`��
	debugRenderer->DrawCone(this->GetTransform()->GetPosition(), coneDirection_, radius_, height_, DirectX::XMFLOAT4{ 0,0,0,1 });

}

void Player::DrawDummyRay()
{
	if (ImGui::TreeNode(u8"RayHIt"))
	{
		//ImGui::Checkbox("DummyHitReset", &isDummyReset_);
		//ImGui::Checkbox("DummyHit", &isDummyHit_);
		//ImGui::DragFloat("RayLimit", &dummyRayLimit_, 0.1f, -FLT_MAX, FLT_MAX);
		//ImGui::DragFloat("RayDebugOffset", &debugOffset_, 0.1f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat3("HitPos", &hitPosition_.x, 0.01f, -FLT_MAX, FLT_MAX);				//	���������ʒu
		ImGui::DragFloat3("HitNormal", &hitNormal_.x, 0.01f, -FLT_MAX, FLT_MAX);			//	���������ʂ̖@��
		ImGui::Text("Mesh", &hitMesh_);														//	���b�V����
		ImGui::Text("Material", &hitMaterial_);												//	�}�e���A����

		ImGui::TreePop();
	}

}