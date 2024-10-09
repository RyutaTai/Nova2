#include "GameObject.h"

#include "../Graphics/Graphics.h"
#include "../Core/Framework.h"

//	�R���X�g���N�^
GameObject::GameObject(const std::string& fileName)
{
	
}

#if 0
//	�W���C���g�|�W�V�����擾
DirectX::XMFLOAT3 GameObject::GetJointPosition(const std::string& meshName, const std::string& boneName, const Animation::Keyframe* keyframe, const DirectX::XMFLOAT4X4& transform)
{
	return modelResource_->GetJointPosition(meshName, boneName, keyframe, transform);
}

DirectX::XMFLOAT3 GameObject::GetJointPosition(const std::string& meshName, const std::string& boneName, const DirectX::XMFLOAT4X4& transform)
{
	return modelResource_->GetJointPosition(meshName, boneName, transform);
}

DirectX::XMFLOAT3 GameObject::GetJointPosition(size_t meshIndex, size_t boneIndex, const Animation::Keyframe* keyframe, const DirectX::XMFLOAT4X4& transform)
{
	return modelResource_->GetJointPosition(meshIndex, boneIndex, keyframe, transform);
}

//	�u�����h��weight�l���Z
void GameObject::AddWeight(float addWeight)
{
	float weight = modelResource_->GetWeight();
	modelResource_->SetWeight(weight + addWeight);
}

//	�u�����h��weight�l���Z
void GameObject::SubtractWeight(float subtractWeight)
{
	float weight = modelResource_->GetWeight();
	modelResource_->SetWeight(weight - subtractWeight);
}

//	�A�j���[�V����
void GameObject::PlayAnimation(int index, bool loop, float speed, float blendSeconds)
{
	modelResource_->PlayAnimation(index, loop, speed, blendSeconds);
}

//	�u�����h�A�j���[�V����
void GameObject::PlayBlendAnimation(int index1, int index2, bool loop, float speed)
{
	modelResource_->PlayBlendAnimation(index1, index2, loop, speed);
}

//	�A�j���[�V�����X�V����
void GameObject::UpdateAnimation(float elapsedTime)
{
	modelResource_->UpdateAnimation(elapsedTime);
}

//	�u�����h�A�j���[�V�����X�V����
void GameObject::UpdateBlendAnimation(float elapsedTime)
{
	modelResource_->UpdateBlendAnimation(elapsedTime);
}
#endif