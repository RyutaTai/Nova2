#include "GameStaticObject.h"

#include "../Graphics/Graphics.h"

GameStaticObject::GameStaticObject(const std::string& fileName, const bool setColor, const DirectX::XMFLOAT4 color)
{
	//	モデル読み込み
	gltfStaticModelResource_ = ResourceManager::Instance().LoadGltfModelStaticResource(fileName, setColor, color);
}

//	ピクセルシェーダー設定
void GameStaticObject::SetPixelShader(const char* csoName)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	Graphics::Instance().GetShader()->CreatePsFromCso(device, csoName, pixelShader_.ReleaseAndGetAddressOf());
	gltfStaticModelResource_->SetPixelShader(pixelShader_.Get());
}

//	描画処理
void GameStaticObject::Render()
{
	DirectX::XMFLOAT4X4 transform = {};
	DirectX::XMMATRIX Transform = GetTransform()->CalcWorld();
	DirectX::XMStoreFloat4x4(&transform, Transform);
	gltfStaticModelResource_->Render(Transform);
}

//	デバッグ描画
void GameStaticObject::DrawDebug()
{
	GetTransform()->DrawDebug();
	gltfStaticModelResource_->DrawDebug();
}
