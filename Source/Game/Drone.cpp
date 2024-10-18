#include "Drone.h"

#include "../Nova/Graphics/Graphics.h"
#include "../Nova/Input/Input.h"
#include "../Nova/Core/Framework.h"
#include "../Nova/Others/MathHelper.h"
#include "../Nova/Others/Converter.h"
#include "DroneState.h"
#include "BulletStraight.h"
#include "BulletHorming.h"
#include "Player.h"

//	�R���X�g���N�^
Drone::Drone()
	:Enemy("./Resources/Model/Drone/Drone.glb")
{
	//	�X�e�[�g�Z�b�g(Drone::StateType�̏��ƍ��킹��)
	stateMachine_.reset(new StateMachine<State<Drone>>());
	stateMachine_->RegisterState(new DroneState::IdleState(this));			//	�ҋ@
	stateMachine_->RegisterState(new DroneState::SearchState(this));		//	�T��
	stateMachine_->RegisterState(new DroneState::MoveState(this));			//	�ړ�
	stateMachine_->RegisterState(new DroneState::AttackState(this));		//	�U��
	stateMachine_->RegisterState(new DroneState::AvoidanceState(this));		//	���

	//	�����X�e�[�g�ݒ�
	stateMachine_->SetState(static_cast<int>(StateType::Idle));

}

Drone::~Drone()
{
	if (sources_[static_cast<int>(Audio3D::Shot)])
	{
		delete sources_[static_cast<int>(Audio3D::Shot)];
	}
}

//	������
void Drone::Initialize()
{
	//	�A�j���[�V�����Z�b�g
	myType_ = ENEMY_TYPE::DRONE;

	//	�����ʒu�ݒ�
	/*DirectX::XMFLOAT3 position = { 190,700,1620 };
	GetTransform()->SetPosition(position);*/

	//	�����p�x�ݒ�
	float angleY = ConvertToRadian(220.0f);
	GetTransform()->SetRotationY(angleY);

	//	�X�P�[��
	float scale = 1.0f;
	//float scale = 10.0f;
	GetTransform()->SetScaleFactor(scale);

	//	���a�ݒ�
	height_ = 4.4f;
	radius_ = 2.5f;

	//	���G�͈͐ݒ�
	searchRange_ = 15.0f;

	//	HP�ݒ�
	hp_ = MAX_HP;

	//	�e�ۏ�����
	BulletManager::Instance().Initialize();

	//	�G�t�F�N�g�ǂݍ���
	effectResource_ = ResourceManager::Instance().LoadEffectResource("./Resources/Effect/HitEff.efk");

	//	�G�t�F�N�g�X�P�[���ݒ�
	effectScale_ = 80.0f;

	/* ----- �I�[�f�B�I������ ----- */
#if 1
	DirectX::XMFLOAT3 playerPos = Player::Instance().GetTransform()->GetPosition();
	float playerHeight = Player::Instance().GetHeight();
	float posOffsetY = -10.0f;
	emitter_[static_cast<int>(Audio3D::Shot)].position_ = GetTransform()->GetPosition();
	//emitter_[static_cast<int>(Audio3D::Shot)].position.y = playerPos.y + playerHeight / 2.0f + posOffsetY;
	emitter_[static_cast<int>(Audio3D::Shot)].velocity_ = { 1.0f, 2.0f, 1.0f };
	emitter_[static_cast<int>(Audio3D::Shot)].minDistance_ = 7.0f;
	emitter_[static_cast<int>(Audio3D::Shot)].maxDistance_ = 12.0f;
	emitter_[static_cast<int>(Audio3D::Shot)].volume_ = 2.0f;
	sources_[static_cast<int>(Audio3D::Shot)] = Audio::Instance().LoadAudioSource3D("./Resources/Audio/SE/shot.wav", &emitter_[static_cast<int>(Audio3D::Shot)]);
	//sources_[static_cast<int>(Audio3D::Shot)] = Audio::Instance().LoadAudioSource3D("./Resources/Audio/BGM/Title.wav", &emitter_[static_cast<int>(Audio3D::Shot)]);
#endif

	//	���ˉ��Đ�
#if 0
	if (sources_[static_cast<int>(Audio3D::Shot)])
	{
		sources_[static_cast<int>(Audio3D::Shot)]->Play(true);
	}
#endif


}

//	�X�V����
void Drone::Update(const float& elapsedTime)
{
	//	�X�e�[�g���̍X�V����
	stateMachine_->Update(elapsedTime);

	//	���̒e�𔭎˂���܂ł̃^�C�}�[�X�V
	launchTimer_ -= elapsedTime;

	//	�v���C���[����������
	if (SearchPlayer())
	{
		//	�e�ۏ���
		LaunchBullet();

	}

	//	���񏈗�
	Turn(elapsedTime);

	//	�e�ۂ������
	if (BulletManager::Instance().GetBulletCount() > 0)
	{
		//	�e�ۍX�V����
		BulletManager::Instance().Update(elapsedTime);
		BulletManager::Instance().CoverModelUpdate(elapsedTime);
	}

	//	HP���Ȃ��Ȃ�����
	if (hp_ <= 0)
	{
		Destroy();
	}

	//	�G�~�b�^�[�X�V
	UpdateEmitter();
	UpdateAudioSource(elapsedTime);
	
}

//	�G�~�b�^�[�X�V
void Drone::UpdateEmitter()
{
	emitter_[static_cast<int>(Audio3D::Shot)].position_ = GetTransform()->GetPosition();
	//emitter_[static_cast<int>(Audio_3d::Shot)].velocity = {1,2,1};

}

//	�I�[�f�B�I�\�[�X�X�V
void Drone::UpdateAudioSource(const float& elapsedTime)
{
	if (sources_[static_cast<int>(Audio3D::Shot)])
	{
		sources_[static_cast<int>(Audio3D::Shot)]->SetDSPSetting(Player::Instance().GetListener());
		sources_[static_cast<int>(Audio3D::Shot)]->Update(elapsedTime);
	}

}

//	�e�ۏ���
void Drone::LaunchBullet()
{
	if (bulletLaunch_)	//	�e�۔��˃t���O�������Ă�����(�f�o�b�O�p)
	{

#if 1
		if (launchTimer_ <= 0.0f)	//	���Ԋu�Œe�𔭎�
#else
		GamePad gamePad = Input::Instance().GetGamePad();
		if (gamePad.GetButtonDown() & GamePad::BTN_START)	//	Enter�L�[�Ŕ���
#endif
		{
			//	�O����
			DirectX::XMFLOAT3 dir = {};
			float angleY = GetTransform()->GetRotationY();

			dir.x = sinf(angleY);
			dir.y = 0.0f;
			dir.z = cosf(angleY);

			//	���ˈʒu
			DirectX::XMFLOAT3 pos = this->GetTransform()->GetPosition();
			pos = pos + dir * 2.0f;

			//	�e�ۃt�@�C����
			const char* bulletName = "./Resources/Model/Bullet/Sphere.glb";
#if  0	//	���i����e�ې���
			BulletStraight* bullet = new BulletStraight(bulletName);
			bullet->Launch(dir, pos);

#else	//	�Ǐ]����e�ې���
			BulletHorming* bullet = new BulletHorming(bulletName);
			bullet->Launch(dir, pos);
#endif	
			//	���L�҂̈ʒu�ݒ�
			bullet->SetOwnerPosition(this->GetTransform()->GetPosition());

			//	���˃^�C�}�[���Z�b�g
			launchTimer_ = 3.0f;

			//	���ˉ��Đ�
			if (sources_[static_cast<int>(Audio3D::Shot)])
			{
				sources_[static_cast<int>(Audio3D::Shot)]->Play(false);
			}

		}
	}
}

//	���񏈗�
void Drone::Turn(const float& elapsedTime)
{
	//	�v���C���[���^�[�Q�b�g�ɐݒ�
	DirectX::XMFLOAT3 playerPos = Player::Instance().GetTransform()->GetPosition();
	SetTargetPosition(playerPos);

	//	�^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o(�P�ʃx�N�g������Turn�֐����ōs���Ă���)
	DirectX::XMFLOAT3 dronePos = this->GetTransform()->GetPosition();
	float vx = targetPosition_.x - dronePos.x;
	float vz = targetPosition_.z - dronePos.z;


	//	�h���[���p�̉�]����
#if 0
	DirectX::XMVECTOR TargetPos = DirectX::XMLoadFloat3(&targetPosition_);				//	�^�[�Q�b�g�̈ʒu�x�N�g��
	DirectX::XMVECTOR DronePos = DirectX::XMLoadFloat3(&GetTransform()->GetPosition());	//	�h���[���̈ʒu�x�N�g��
	DirectX::XMVECTOR DroneToTarget = DirectX::XMVectorSubtract(TargetPos, DronePos);	//	�h���[������^�[�Q�b�g�ւ̃x�N�g��
	DroneToTarget = DirectX::XMVector3Normalize(DroneToTarget);							//	�^�[�Q�b�g�ւ̃x�N�g���𐳋K��	
	DirectX::XMVECTOR Front = DirectX::XMLoadFloat3(&GetTransform()->CalcForward());	//	�h���[���̑O����
	Front = DirectX::XMVector3Normalize(Front);											//	�h���[���̑O�����x�N�g���𐳋K��
	
	DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Front, DroneToTarget);				//	�O�ς��s���A��]�����Z�o
	float rad = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Front, DroneToTarget));		//	���ς���
	rad = acosf(rad);	//	���ς̌��ʂ����]�p�x�����߂�

	if (fabsf(rad) > 1e-8f)
	{
		//DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationAxis(Axis, rad * elapsedTime);											//	��]��Axis�Ɖ�]�p�xrad�����]�N�I�[�^�j�I�������߂�
		//DirectX::XMVECTOR Rotate = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&GetTransform()->GetRotation()), Q);		//	���߂��N�I�[�^�j�I�����������킹��
		//DirectX::XMFLOAT4 rotation;
		//DirectX::XMStoreFloat4(&rotation, Rotate);
		//GetTransform()->SetRotation(rotation);

		DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationAxis(Axis, rad);														//	��]��Axis�Ɖ�]�p�xrad�����]�N�I�[�^�j�I�������߂�
		DirectX::XMVECTOR Rotate = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&GetTransform()->GetRotation()), Q);		//	���߂��N�I�[�^�j�I�����������킹��
		DirectX::XMFLOAT4 rotation;
		DirectX::XMStoreFloat4(&rotation, Rotate);
		GetTransform()->SetRotation(rotation);
	}

#endif

	//	���񏈗�
#if 1
	Character::Turn(elapsedTime, vx, vz, turnSpeed_);
#endif
}

//	�X�e�[�W�Ƃ̓����蔻��
bool Drone::RayVsVertical(const float& elapsedTime)
{

	return false;
}

bool Drone::RayVsHorizontal(const float& elapsedTime)
{

	return false;
}

//	�j������
void Drone::Destroy()
{
	//	�G�t�F�N�g�Đ�
	DirectX::XMFLOAT3 effectPos = GetTransform()->GetPosition();
	effectResource_->Play(effectPos, effectScale_);
	
	Enemy::Destroy();	//	���g��j��
}

//	�`�揈��
void Drone::Render()
{
	//	�h���[���`��
	//	�s�N�Z���V�F�[�_�[�Z�b�g
	//SetPixelShader("./Resources/Shader/DronePS.cso");
	Character::Render();

	//	�e�ە`��
	BulletManager::Instance().Render();

}

//	�f�o�b�O�v���~�e�B�u�`��
void Drone::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//	�Փ˔���p�̃f�o�b�O����`��
	debugRenderer->DrawCylinder(this->GetTransform()->GetPosition(), radius_, height_, DirectX::XMFLOAT4(0, 0, 0, 1));

	//	���G�͈͕`��(�~��)
	debugRenderer->DrawCylinder(this->GetTransform()->GetPosition(), searchRange_, 1.0f, { 0,1,0.1f,1.0f });
	
	//	�e�ۂ̃f�o�b�O���`��
	BulletManager::Instance().DrawDebugPrimitive();

}

//	�f�o�b�O�`��
void Drone::DrawDebug()
{
	float scale = GetTransform()->GetScaleFactor();

	if (ImGui::TreeNode(u8"Drone �h���[��"))
	{
		GetTransform()->DrawDebug();
		Character::DrawDebug();
		ImGui::DragFloat("ScaleFactor", &scale,1.0f, -FLT_MAX, FLT_MAX);	//	�X�P�[��
		ImGui::DragFloat("TurnSpeed", &turnSpeed_, 1.0f, -FLT_MAX, FLT_MAX);	//	���񑬓x
		ImGui::DragFloat("SerchRange", &searchRange_, 0.1f, -FLT_MAX, FLT_MAX);
		ImGui::Checkbox("Bullet Launch ", &bulletLaunch_);						//	�e�۔���
		ImGui::TreePop();
	}
	BulletManager::Instance().DrawDebug();	//	�e��ImGui
	
	GetTransform()->SetScaleFactor(scale);

}