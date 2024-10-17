#include "Stage.h"

#include "../Nova/Graphics/Graphics.h"

static Stage* instance = nullptr;

Stage::Stage(const std::string& filename)
	: GameStaticObject(filename)
{
	//	インスタンスポイント設定
	instance = this;

	//	コリジョンメッシュ生成
#if 0
	//collisionMesh_ = std::make_unique<decltype(collisionMesh_)::element_type>(Graphics::Instance().GetDevice(), "./Resources/Model/syougiban.glb");
	collisionMesh_ = std::make_unique<decltype(collisionMesh_)::element_type>(Graphics::Instance().GetDevice(), "./Resources/Model/cybercity-2099-v2/city_collision_ground2_correct.glb");
#else
	
	GetTransform()->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	GetTransform()->SetScaleFactor(0.0025f);	//	シティモデル

	collisionMesh_ = std::make_unique<decltype(collisionMesh_)::element_type>(Graphics::Instance().GetDevice(), "./Resources/Model/cybercity-2099-v2/city.glb");

	//DirectX::XMFLOAT4X4 transform = {};
	//DirectX::XMStoreFloat4x4(&transform, GetTransform()->CalcWorldMatrix(GetTransform()->GetScaleFactor()));
	//collisionMesh_->Transform(transform);
#endif

	//GetTransform()->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

}

//	インスタンス取得
Stage& Stage::Instance()
{
	return *instance;
}


Stage::~Stage()
{

}

//	コリジョンメッシュの当たり判定
bool Stage::Collision(_In_ DirectX::XMFLOAT3 rayStartPosition, _In_ DirectX::XMFLOAT3 rayDirection, _In_ const DirectX::XMFLOAT4X4& transform, _Out_ DirectX::XMFLOAT3& intersectionPosition, _Out_ DirectX::XMFLOAT3& intersectionNormal,
	_Out_ std::string& intersectionMesh, _Out_ std::string& intersectionMaterial, _In_ float rayLengthLimit, _In_ bool skipIf) const
{
#if 0
	if (collisionMesh_->Raycast(rayStartPosition, rayDirection, transform, intersectionPosition, intersectionNormal, intersectionMesh, intersectionMaterial, rayLengthLimit, skipIf))
	{
		OutputDebugStringA("Position:");
		OutputDebugStringA("Intersected : ");
		OutputDebugStringA(intersectionMesh.c_str());
		OutputDebugStringA(" : ");
		OutputDebugStringA(intersectionMaterial.c_str());
		OutputDebugStringA("\n");
		return true;
	}
	else
	{
		OutputDebugStringA("Unintersected...\n");
		return false;
	}
#else
	//	空間分割
	if (collisionMesh_->RaycastWithSpaceDivision(rayStartPosition, rayDirection, transform, intersectionPosition, intersectionNormal, intersectionMesh, intersectionMaterial, rayLengthLimit, skipIf))
	{
		OutputDebugStringA("Position:");
		OutputDebugStringA("Intersected : ");
		OutputDebugStringA(intersectionMesh.c_str());
		OutputDebugStringA(" : ");
		OutputDebugStringA(intersectionMaterial.c_str());
		OutputDebugStringA("\n");
		return true;
	}
	else
	{
		OutputDebugStringA("Unintersected...\n");
		return false;
	}
#endif
}

//	Shadow描画
void Stage::ShadowRender(const float& scale)
{
#if 0
	ID3D11RenderTargetView* null_render_target_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	Graphics::Instance().GetDeviceContext()->OMSetRenderTargets(_countof(null_render_target_views), null_render_target_views, 0);
	ID3D11ShaderResourceView* null_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	Graphics::Instance().GetDeviceContext()->VSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);
	Graphics::Instance().GetDeviceContext()->PSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);
#endif
	ID3D11PixelShader* nullPixelShader{ NULL };

	//gltfStaticModelResource_->Render(GetTransform()->CalcWorldMatrix(scale));
	//gltfStaticModelResource_->Render(GetTransform()->CalcWorldMatrix(scale), GetTransform()->GetColor(), nullptr, &nullPixelShader);

}

//	描画処理
void Stage::Render()
{
	//	ピクセルシェーダーセット
	SetPixelShader("./Resources/Shader/CityPS.cso");
	GameStaticObject::Render();

}

//	デバッグ描画
void Stage::DrawDebug()
{
	if (ImGui::TreeNode(u8"Stageステージ"))
	{
		gltfStaticModelResource_->DrawDebug();
		GetTransform()->DrawDebug();
		ImGui::TreePop();
	}
}
