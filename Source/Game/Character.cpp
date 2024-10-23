#include "Character.h"

#include "../Nova/Core/Framework.h"
#include "Character.h"
#include "../Nova/Others/MathHelper.h"
#include "Stage.h"

//	�R���X�g���N�^
Character::Character(const std::string& filename, const std::string& rootNodeName)
{
	//	���f���ǂݍ���
	gltfModelResource_ = ResourceManager::Instance().LoadGltfModelResource(filename, rootNodeName);
}

//	�x���V�e�B�X�V
void Character::UpdateVelocity(const float& elapsedTime)
{
	//#if 0 //	�L�����̑O�����Ɉړ������鏈��
	//	DirectX::XMFLOAT3 front = this->GetTransform()->CalcForward();	//	�L�����N�^�[�̑O�����擾
	//	DirectX::XMVECTOR frontV = DirectX::XMLoadFloat3(&front);
	//	frontV = DirectX::XMVector3Normalize(frontV);
	//	DirectX::XMStoreFloat3(&front, frontV);
	//
	//	float range = 10.0f;
	//	velocity_ = front * range * Framework::tictoc_.GetDeltaTime();
	//	this->GetTransform()->AddPosition(velocity_);
	//#endif

#if 0
	float vecLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&moveVec_)));
	if (vecLength > 0)
	{
		velocity_ = moveVec_ * moveSpeed_ * elapsedTime;
	}
#else
	velocity_ = moveVec_ * moveSpeed_ * elapsedTime + acceleration_ * elapsedTime;

#endif
}

//	�x���V�e�B���Z
void Character::AddVelocity(const DirectX::XMFLOAT3& addVelocity,const float& elapsedTime)
{
	DirectX::XMVECTOR Velocity = DirectX::XMLoadFloat3(&velocity_);
	DirectX::XMVECTOR AddVelocity = DirectX::XMLoadFloat3(&addVelocity);
	Velocity = DirectX::XMVectorAdd(Velocity, AddVelocity);
	//Velocity = DirectX::XMVectorScale(Velocity, elapsedTime);
	DirectX::XMStoreFloat3(&velocity_, Velocity);
}

void Character::AddVelocityY(const float& addVelocityY, const float& elapsedTime)
{
	velocity_.y += addVelocityY * elapsedTime;
}

void Character::AddVelocityXZ(const float& addVelocityX, const float& addVelocityZ, const float& elapsedTime)
{
	velocity_.x += addVelocityX * elapsedTime;
	velocity_.z += addVelocityZ * elapsedTime;
}

//	XZ������Velocity�݂̂Ɉ����̐����|����
void Character::MultiplyVelocityXZ(const float& multiplyVelocity, const float& elapsedTime)
{
	velocity_.x *= multiplyVelocity * elapsedTime;
	velocity_.z *= multiplyVelocity * elapsedTime;
}

//	�����x���Z
void Character::AddAcceleration(const DirectX::XMFLOAT3& addAcceleration, const float& elapsedTime)
{
	DirectX::XMVECTOR Acceleration = DirectX::XMLoadFloat3(&acceleration_);
	DirectX::XMVECTOR AddAcceleration = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&addAcceleration), elapsedTime);
	Acceleration = DirectX::XMVectorAdd(Acceleration, AddAcceleration);
	DirectX::XMStoreFloat3(&acceleration_, Acceleration);
}

void Character::AddAccelerationY(const float& addAccelerationY, const float& elapsedTime)
{
	acceleration_.y += addAccelerationY * elapsedTime;
}

void Character::AddAccelerationXZ(const float& addAccelerationX, const float& addAccelerationZ, const float& elapsedTime)
{
	acceleration_.x += addAccelerationX * elapsedTime;
	acceleration_.z += addAccelerationZ * elapsedTime;
}

void Character::AddMoveSpeed(const float& addMoveSpeed, const float& elapsedTime)
{
	//	�ő�X�s�[�h�𒴂��Ă��Ȃ��ꍇ�̂݉��Z����
	if (moveSpeed_ < MOVE_SPEED)
	{
		moveSpeed_ += addMoveSpeed * elapsedTime;
	}
	else if (MOVE_SPEED >= moveSpeed_)
	{
		moveSpeed_ = MOVE_SPEED;
	}
}

//	�ړ�����
void Character::Move(const float& elapsedTime)
{
	this->GetTransform()->AddPosition(velocity_);
}

//	���񏈗�
void Character::Turn(const float& elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	//	�i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
	float length;
	length = sqrtf(vx * vx + vz * vz);

	if (length <= 0)
	{
		return;
	}

	//	�i�s�x�N�g����P�ʃx�N�g����
	vx /= length;
	vz /= length;

	//	���g�̉�]�l����O���������߂�
	float angleY = this->GetTransform()->GetRotationY();
	float frontX = sinf(angleY);
	float frontZ = cosf(angleY);

	//	���E������s�����߂ɂQ�̒P�ʃx�N�g���̊O�ς��v�Z����
	float cross = (frontZ * vx) - (frontX * vz);

	//	��]�p�����߂邽�߁A�Q�̒P�ʃx�N�g���̓��ς��v�Z����
	float dot = (frontX * vx) + (frontZ * vz);

	//	���ϒl��-1.0�`1.0�ŕ\������Ă���B
	//	�Q�̒P�ʃx�N�g���̊p�x���������ق�
	//	1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
	float rot = 1.0 - dot;
	if (rot > speed)rot = speed;

	//	2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
	//	���E������s�����Ƃɂ���č��E��]��I������
	//	���̏ꍇ�͉E�A���̏ꍇ�͍�
	if (cross < 0.0f)//	��
	{
		this->GetTransform()->AddRotationY(-rot);
	}
	else//	�E
	{
		this->GetTransform()->AddRotationY(rot);
	}
}

//	�A�j���[�V�����Đ�
void Character::PlayAnimation(const int& index, const bool& loop, const float& speed, const float& blendTime,const float& startFrame)
{
	gltfModelResource_->PlayAnimation(index, loop, speed, blendTime, startFrame);
}

//	�A�j���[�V�����X�V����
void Character::UpdateAnimation(const float& elapsedTime)
{
	gltfModelResource_->UpdateAnimation(elapsedTime);
}

//	�A�j���[�V�����Đ������ǂ���
bool Character::IsPlayAnimation()const
{
	return gltfModelResource_->IsPlayAnimation();
}

//	�s�N�Z���V�F�[�_�[�ݒ�
void Character::SetPixelShader(const char* csoName)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	Graphics::Instance().GetShader()->CreatePsFromCso(device, csoName, pixelShader_.ReleaseAndGetAddressOf());
	gltfModelResource_->SetPixelShader(pixelShader_.Get());
}

//	�W���C���g�|�W�V�����擾
DirectX::XMFLOAT3 Character::GetJointPosition(const std::string& meshName, const std::string& boneName, const DirectX::XMFLOAT4X4& transform)
{
	return gltfModelResource_->GetJointPosition(meshName, boneName, transform);
}

DirectX::XMFLOAT3 Character::GetJointPosition(size_t nodeIndex, const DirectX::XMFLOAT4X4& transform)
{
	return gltfModelResource_->GetJointPosition(nodeIndex, transform);
}

//	HP����
void Character::SubtractHp(int hp)
{
	if (isInvincible_ == false)	//	���G����Ȃ�������HP����
	{
		hp_ -= hp;
		if (hp_ <= 0)
		{
			hp_ = 0;
		}
	}
}

//	�A�j���[�V�����ǉ�
void Character::AppendAnimation(const std::string& filename)
{
	gltfModelResource_->AppendAnimation(filename);
}

//	�`�揈��
void Character::Render()
{
	gltfModelResource_->Render(GetTransform()->CalcWorld());
}

//	�f�o�b�O�`��
void Character::DrawDebug()
{
	gltfModelResource_->DrawDebug();
	GetTransform()->DrawDebug();

	ImGui::DragInt("HP", &hp_, 1.0f, 0, INT_MAX);									//	HP
	ImGui::DragFloat3("Velocity", &velocity_.x, 0.01f, -FLT_MAX, FLT_MAX);			//	�ړ����x
	ImGui::DragFloat3("Acceleration", &acceleration_.x, 0.01f, -FLT_MAX, FLT_MAX);	//	�����x
	ImGui::DragFloat3("moveVec", &moveVec_.x, 0.01f, -FLT_MAX, FLT_MAX);			//	�ړ��x�N�g��
	ImGui::DragFloat("Height", &height_, 0.01f, -FLT_MAX, FLT_MAX);					//	����
	ImGui::DragFloat("Radius", &radius_, 0.01f, -FLT_MAX, FLT_MAX);					//	���a

}

