#pragma once

#include "GameObject.h"

class GameStaticObject :public GameObject
{
public:
	GameStaticObject(const std::string& fileName, const bool setColor = false, const DirectX::XMFLOAT4 color = { 0,0,0,1 });
	GameStaticObject() {}
	virtual ~GameStaticObject() {}

	void Render()override;
	virtual void ShadowRender(const float& scale = 1.0f) override{}

	virtual void DrawDebug();

	void SetPixelShader(const char* csoName);			//	ピクセルシェーダー設定

	Transform* GetTransform() { return &transform_; }

private:
	Transform									transform_ = {};
	std::shared_ptr <GltfModelStaticBatching>	gltfStaticModelResource_;
	Microsoft::WRL::ComPtr <ID3D11PixelShader>	pixelShader_;					//	ピクセルシェーダー

};

