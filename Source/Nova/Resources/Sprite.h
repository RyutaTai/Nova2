#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <sstream>
#include <wrl.h>
#include <WICTextureLoader.h>

#include "../Others/Misc.h"
#include "../Graphics/Shader.h"
#include "../Resources/Texture.h"

class Sprite
{
public:	// 構造体
	struct SpriteTransform
	{
	public:
		void CutOut();							//	画像切り取り
		void CutOutX(const float& cutSizeX);	//	X方向切り取り
		void CutOutY(const float& cutSizeY);	//	Y方向切り取り

		void DrawDebug();	//	ImGui

		void SetPosition(const DirectX::XMFLOAT2& pos) { position_ = pos; }
		void SetPosition(const float& x, const float& y)   { position_ = { x, y }; }
		DirectX::XMFLOAT2 GetPosition() { return position_; }
		float GetPositionX() { return position_.x; }
		float GetPositionY() { return position_.y; }

		void SetSize(const DirectX::XMFLOAT2& size)  { size_ = size; }
		void SetSize(const float& x, const float& y) { size_ = { x, y }; }
		void SetSizeX(const float& x) { size_.x = x; }
		void SetSizeY(const float& y) { size_.y = y; }
		DirectX::XMFLOAT2 GetSize() { return size_; }
		float GetSizeX() { return size_.x; }
		float GetSizeY() { return size_.y; }

		void SetTexPos(const DirectX::XMFLOAT2& texPos) { texPos_ = texPos; }
		void SetTexPos(const float& x, const float& y)  { texPos_ = { x, y }; }
		void SetTexPosX(const float& x) { texPos_.x = x; }
		void SetTexPosY(const float& y) { texPos_.y = y; }
		DirectX::XMFLOAT2 GetTexPos() { return texPos_; }
		float GetTexPosX() { return texPos_.x; }
		float GetTexPosY() { return texPos_.y; }

		void SetTexSize(const DirectX::XMFLOAT2 texSize) { texSize_ = texSize; }
		void SetTexSize(const float& x, const float& y)  { texSize_ = { x, y }; }
		void SetTexSizeX(const float& x) { texSize_.x = x; }
		void SetTexSizeY(const float& y) { texSize_.y = y; }
		DirectX::XMFLOAT2 GetTexSize() { return texSize_; }
		float GetTexSizeX() { return texSize_.x; }
		float GetTexSizeY() { return texSize_.y; }

		void SetColor(const DirectX::XMFLOAT4& color) { color_ = color; }
		void SetColor(const float& r, const float& g, const float& b, const float& a) { color_ = { r,g,b,a }; }
		void SetColorR(const float& r) { color_.x = r; }
		void SetColorG(const float& g) { color_.y = g; }
		void SetColorB(const float& b) { color_.z = b; }
		void SetColorA(const float& a) { color_.w = a; }
		DirectX::XMFLOAT4 GetColor() { return color_; }

		void SetAngle(const float& angle) { angle_ = angle; }
		float GetAngle() { return angle_; }

		void SetIsCut(bool isCut) { isCut_ = isCut; }
		bool IsCut() { return isCut_; }

		void SetCutSize(const DirectX::XMFLOAT2& cutSize) { cutSize_ = cutSize; }
		void SetCutSizeX(const float& cutSizeX) { cutSize_.x = cutSizeX; }
		void SetCutSizeY(const float& cutSizeY) { cutSize_.y = cutSizeY; }

	private:
		DirectX::XMFLOAT2 position_ = {};	//	位置 
		DirectX::XMFLOAT2 size_ = {};		//	画像サイズ 
		DirectX::XMFLOAT2 texPos_ = {};		//	切り取り開始位置
		DirectX::XMFLOAT2 texSize_ = {};	//	切り取りサイズ
		DirectX::XMFLOAT4 color_ = {};		//	描画色 
		float angle_ = 0;					//	回転角度

	private:
		bool isCut_ = false;				//	切り取るフラグ
		DirectX::XMFLOAT2 cutSize_ = {};	//	切り取りサイズ

	};

private:
	struct Vertex
	{
		DirectX::XMFLOAT3 position_;
		DirectX::XMFLOAT4 color_;
		DirectX::XMFLOAT2 texcoord_;
	};

public:
	Sprite(const wchar_t* filename);
	~Sprite();

	void Render();

	// 描画実行
	void Render(ID3D11DeviceContext* deviceContext,
		float dx, float dy,
		float dw, float dh,
		float sx, float sy,
		float sw, float sh,
		float angle,
		float r, float g, float b, float a) const;

	void DrawDebug();

	void Textout(std::string s,
		float x, float y, float w, float h, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
	
	SpriteTransform* GetTransform() { return &transform_; }

private:
	Microsoft::WRL::ComPtr <ID3D11Buffer>				vertexBuffer_;
	Microsoft::WRL::ComPtr <ID3D11VertexShader>			vertexShader_;
	Microsoft::WRL::ComPtr <ID3D11PixelShader>			pixelShader_;
	Microsoft::WRL::ComPtr <ID3D11InputLayout>			inputLayout_;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>	shaderResourceView_;
	D3D11_TEXTURE2D_DESC								texture2dDesc_;

	SpriteTransform										transform_ = {};

};
