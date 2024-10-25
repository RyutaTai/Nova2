#pragma once

#include <memory>

#include "../Nova/Resources/GltfModelStaticBatching.h"
#include "../Nova/Collision/CollisionMesh.h"
#include "../Nova/Resources/Midi.h"
#include "../Nova/Audio/Frequency.h"

class Stage
{
public:
	Stage();
	~Stage() {}

	static Stage& Instance();

	void ShadowRender(const float& scale = 1.0f);
	void Update(const float& elapsedTime);
	void Render();
	void DrawDebug();

	bool Collision(_In_ DirectX::XMFLOAT3 rayPosition, _In_ DirectX::XMFLOAT3 rayDirection, _In_ const DirectX::XMFLOAT4X4& transform, _Out_ DirectX::XMFLOAT3& intersectionPosition, _Out_ DirectX::XMFLOAT3& intersectionNormal,
		_Out_ std::string& intersectionMesh, _Out_ std::string& intersectionMaterial, _In_ float rayLengthLimit = 1.0e+7f, _In_ bool skipIf = false/*Once the first intersection is found, the process is interrupted.*/) const;

	Transform* GetTransform() { return gltfStaticModelResource_->GetTransform(); }
	Frequency* GetFrequency() { return frequency_.get(); }	//	音の周波数データ取得

private:
	enum class CollisionModel
	{
		Ground,
		Building,
		MAX,
	};

	struct EmissiveConstants
	{
		float emissiveIntensity_;
		float dummy_[3];
	};
	EmissiveConstants emissiveConstant_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> emissiveConstantBuffer_;

private:
	static Stage* instance_;

	std::shared_ptr<GltfModelStaticBatching>	gltfStaticModelResource_;		//	Gltfモデル
	std::unique_ptr<CollisionMesh>				collisionMesh_;

	float frequencyMax_ = 15000.0f;
	int frequencyIndex_ = 509;
	float emissiveIntencityMin_ = 0.1f;
	float emissiveIntencityMax_ = 15.0f;
	std::unique_ptr<Midi> midi_ = nullptr;				//	emissiveタイミング判定用midi

	std::unique_ptr<Frequency> frequency_ = nullptr;	//	音の周波数データ(emissiveIntencityの計算に使う)
};