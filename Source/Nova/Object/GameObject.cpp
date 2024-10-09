#include "GameObject.h"

#include "../Graphics/Graphics.h"
#include "../Core/Framework.h"

//	コンストラクタ
GameObject::GameObject(const std::string& fileName)
{
	
}

#if 0
//	ジョイントポジション取得
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

//	ブレンドのweight値加算
void GameObject::AddWeight(float addWeight)
{
	float weight = modelResource_->GetWeight();
	modelResource_->SetWeight(weight + addWeight);
}

//	ブレンドのweight値減算
void GameObject::SubtractWeight(float subtractWeight)
{
	float weight = modelResource_->GetWeight();
	modelResource_->SetWeight(weight - subtractWeight);
}

//	アニメーション
void GameObject::PlayAnimation(int index, bool loop, float speed, float blendSeconds)
{
	modelResource_->PlayAnimation(index, loop, speed, blendSeconds);
}

//	ブレンドアニメーション
void GameObject::PlayBlendAnimation(int index1, int index2, bool loop, float speed)
{
	modelResource_->PlayBlendAnimation(index1, index2, loop, speed);
}

//	アニメーション更新処理
void GameObject::UpdateAnimation(float elapsedTime)
{
	modelResource_->UpdateAnimation(elapsedTime);
}

//	ブレンドアニメーション更新処理
void GameObject::UpdateBlendAnimation(float elapsedTime)
{
	modelResource_->UpdateBlendAnimation(elapsedTime);
}
#endif