#pragma once

#include <memory>

#include "../Resources/GltfModelStaticBatching.h"
#include "../Others/Transform.h"
#include "../Resources/ResourceManager.h"

//	FBX
# if 0	
#include "../Resources/Model.h"
class GameObject
{
public:
	GameObject(const char* fbxFileName, bool triangulate = false, float samplingRate = 0);
	GameObject() {}
	virtual ~GameObject() {}

	void ShadowRender(const float& scale = 1.0f);
	void Render(const float& scale = 1.0f, const float& animationSpeed = 1.0f);
	void ModelDrawDebug() { modelResource_->DrawDebug(); }

	// アニメーション関連
	void PlayAnimation(int index, bool loop, float speed = 1.0f, float blendSeconds = 1.0f);
	void PlayBlendAnimation(int index1, int index2, bool loop, float speed = 1.0f);
	void UpdateAnimation(float elapsedTime);
	void UpdateBlendAnimation(float elapsedTime);
	bool IsPlayAnimation() { return modelResource_->IsPlayAnimation(); }

	Transform*	GetTransform()	{ return &transform_; }
	int			GetCurrentBlendAnimationIndex() { return modelResource_->GetCurrentBlendAnimIndex2(); }	//	現在のブレンドアニメーション番号取得
	float		GetBlendRate()	{ return modelResource_->GetBlendRate(); }				//	ブレンド率取得
	float		GetWeight()		{ return modelResource_->GetWeight(); }						//	ブレンドのweight値取得
	Animation::Keyframe& GetKeyFrame() { return modelResource_->GetKeyFrame(); }		//	キーフレーム取得
	DirectX::XMFLOAT3 GetJointPosition(const std::string& meshName, const std::string& boneName, const Animation::Keyframe* keyframe, const DirectX::XMFLOAT4X4& transform);	//	ジョイントポジション取得
	DirectX::XMFLOAT3 GetJointPosition(const std::string& meshName, const std::string& boneName, const DirectX::XMFLOAT4X4& transform);											//	ジョイントポジション取得
	DirectX::XMFLOAT3 GetJointPosition(size_t meshIndex, size_t boneIndex, const Animation::Keyframe* keyframe, const DirectX::XMFLOAT4X4& transform);							//	ジョイントポジション取得

	void SetPixelShader(ID3D11PixelShader* pixelShader);								//	PixelShader設定
	void SetBlendRate(float blendRate)	{ modelResource_->SetBlendRate(blendRate); }	//	ブレンド率設定
	void SetWeight(float weight)		{ modelResource_->SetWeight(weight); }			//	ブレンドのweight値設定
	void AddWeight(float addWeight);													//	ブレンドのweight値加算
	void SubtractWeight(float subtractWeight);

	//Model* GetModel() { return modelResource_.get(); }								//	モデル取得

protected:
	Microsoft::WRL::ComPtr	<ID3D11PixelShader>	pixelShader_;

private:
	std::shared_ptr <Model> modelResource_;
	Transform				transform_		= {};
	//Animation::Keyframe		keyframe_		= {};
	//float					animationTick_	= {};

};
#endif

class GameObject
{
public:
	GameObject(const std::string& fileName);
	GameObject() {}
	virtual ~GameObject() {}

	virtual void Render() = 0;
	//virtual void Render(const float& scale = 1.0f) = 0;
	virtual void ShadowRender(const float& scale = 1.0f) {}

	Transform* GetTransform() { return &transform_; }

	// アニメーション関連
#if 0
	void PlayAnimation(int index, bool loop, float speed = 1.0f, float blendSeconds = 1.0f);
	void PlayBlendAnimation(int index1, int index2, bool loop, float speed = 1.0f);
	void UpdateAnimation(float elapsedTime);
	void UpdateBlendAnimation(float elapsedTime);
	bool IsPlayAnimation() { return gltfModelResource_->IsPlayAnimation(); }

	int			GetCurrentBlendAnimationIndex() { return gltfModelResource_->GetCurrentBlendAnimIndex2(); }	//	現在のブレンドアニメーション番号取得
	float		GetBlendRate() { return gltfModelResource_->GetBlendRate(); }				//	ブレンド率取得
	float		GetWeight() { return gltfModelResource_->GetWeight(); }						//	ブレンドのweight値取得
	Animation::Keyframe& GetKeyFrame() { return gltfModelResource_->GetKeyFrame(); }		//	キーフレーム取得
	DirectX::XMFLOAT3 GetJointPosition(const std::string& meshName, const std::string& boneName, const Animation::Keyframe* keyframe, const DirectX::XMFLOAT4X4& transform);	//	ジョイントポジション取得
	DirectX::XMFLOAT3 GetJointPosition(const std::string& meshName, const std::string& boneName, const DirectX::XMFLOAT4X4& transform);											//	ジョイントポジション取得
	DirectX::XMFLOAT3 GetJointPosition(size_t meshIndex, size_t boneIndex, const Animation::Keyframe* keyframe, const DirectX::XMFLOAT4X4& transform);							//	ジョイントポジション取得

	void SetBlendRate(float blendRate) { gltfModelResource_->SetBlendRate(blendRate); }	//	ブレンド率設定
	void SetWeight(float weight) { gltfModelResource_->SetWeight(weight); }			//	ブレンドのweight値設定
	void AddWeight(float addWeight);													//	ブレンドのweight値加算
	void SubtractWeight(float subtractWeight);
#endif

private:
	Transform				transform_ = {};
	//std::shared_ptr <GltfModelStaticBatching> gltfModelResource_;
	//Animation::Keyframe		keyframe_		= {};
	//float					animationTick_	= {};

};

