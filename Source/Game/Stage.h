#pragma once

#include "../Nova/Object/GameStaticObject.h"
#include "../Nova/Collision/CollisionMesh.h"

class Stage : public GameStaticObject
{
public:
	Stage(const std::string& fileName);
	Stage(){}
	~Stage();

	static Stage& Instance();

	void ShadowRender(const float& scale = 1.0f)override;
	void Render();
	void DrawDebug();

	bool Collision(_In_ DirectX::XMFLOAT3 rayPosition, _In_ DirectX::XMFLOAT3 rayDirection, _In_ const DirectX::XMFLOAT4X4& transform, _Out_ DirectX::XMFLOAT3& intersectionPosition, _Out_ DirectX::XMFLOAT3& intersectionNormal,
		_Out_ std::string& intersectionMesh, _Out_ std::string& intersectionMaterial, _In_ float rayLengthLimit = 1.0e+7f, _In_ bool skipIf = false/*Once the first intersection is found, the process is interrupted.*/) const;

private:
	enum class CollisionModel
	{
		Ground,
		Building,
		MAX,
	};

private:
	std::shared_ptr <GltfModelStaticBatching>	gltfStaticModelResource_;		//	Gltfƒ‚ƒfƒ‹
	std::unique_ptr <CollisionMesh>				collisionMesh_;
	//std::unique_ptr <CollisionMesh>				collisionMesh_[static_cast<int>(CollisionModel::MAX)];

};