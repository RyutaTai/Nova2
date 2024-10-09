#include "GameStaticObject.h"

#include "../Graphics/Graphics.h"

GameStaticObject::GameStaticObject(const std::string& fileName, const bool setColor, const DirectX::XMFLOAT4 color)
{
	//	���f���ǂݍ���
	gltfStaticModelResource_ = ResourceManager::Instance().LoadGltfModelStaticResource(fileName, setColor, color);
}

//	�s�N�Z���V�F�[�_�[�ݒ�
void GameStaticObject::SetPixelShader(const char* csoName)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	Graphics::Instance().GetShader()->CreatePsFromCso(device, csoName, pixelShader_.ReleaseAndGetAddressOf());
	gltfStaticModelResource_->SetPixelShader(pixelShader_.Get());
}

//	�`�揈��
void GameStaticObject::Render()
{
	DirectX::XMFLOAT4X4 transform = {};
	DirectX::XMMATRIX Transform = GetTransform()->CalcWorld();
	DirectX::XMStoreFloat4x4(&transform, Transform);
	gltfStaticModelResource_->Render(Transform);
}

//	�f�o�b�O�`��
void GameStaticObject::DrawDebug()
{
	GetTransform()->DrawDebug();
	gltfStaticModelResource_->DrawDebug();
}
