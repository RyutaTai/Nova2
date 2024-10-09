#include "Dragonkin.h"

#include "../Nova/Graphics/Graphics.h"

//	�R���X�g���N�^
Dragonkin::Dragonkin()
	:Enemy("./Resources/Model/silver-dragonkin-mir4/source/Silver_Dragonkin/Mon_BlackDragon31_Skeleton2")
{
	
}

//	������
void Dragonkin::Initialize()
{
	myType_ = ENEMY_TYPE::DRAGONKIN;
	//SetAnimation(DragonkinAnimation::ANIM_IDLE02);	//	�ҋ@�A�j���[�V�����Z�b�g
}

//	�X�V����
void Dragonkin::Update(const float& elapsedTime)
{

}

//	�X�e�[�W�Ƃ̓����蔻��
bool Dragonkin::RayVsVertical(const float& elapsedTime)
{

	return false;
}

bool Dragonkin::RayVsHorizontal(const float& elapsedTime)
{

	return false;
}

//	�`�揈��
void Dragonkin::Render()
{
	//	�s�N�Z���V�F�[�_�[�Z�b�g
	//Graphics::Instance().GetShader()->CreatePsFromCso(Graphics::Instance().GetDevice(), "./Resources/Shader/DronePS.cso", pixelShader_.ReleaseAndGetAddressOf());
	//this->SetPixelShader(pixelShader_.Get());

	Character::Render();
}

//	�f�o�b�O�v���~�e�B�u�`��
void Dragonkin::DrawDebugPrimitive()
{
	//DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
	//
	////	�Փ˔���p�̃f�o�b�O����`��
	//debugRenderer->DrawSphere(this->GetTransform()->GetPosition(), radius_, DirectX::XMFLOAT4(0, 0, 0, 1));
	//debugRenderer->DrawSphere(this->GetTransform()->GetPosition(), radius_, DirectX::XMFLOAT4(0, 0, 0, 1));
	//
	////	���G�͈͕`��(�~��)
	//debugRenderer->DrawCylinder(this->GetTransform()->GetPosition(), searchRange_, 30.0f, { 0,1,0.1f,1.0f });

}

//	�f�o�b�O�`��
void Dragonkin::DrawDebug()
{
	if (ImGui::TreeNode(u8"Dragonkin���l"))
	{
		//GetTransform()->DrawDebug();
		ImGui::DragFloat3("moveVec", &moveVec_.x, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat("AnimationSpeed", &animationSpeed_, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::TreePop();
	}
}