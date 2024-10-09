#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>

#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"

class Transform
{
public:
	enum class CoordinateSystem
	{
		cRightYup,
		cLeftYup,
		cRightZup,
		cLeftZup,
		cNone,
	};

	Transform() :
		position_(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f))
		, scale_(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f))
		, rotation_(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f))
		, color_(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
		, scaleFactor_(1.0f)
		, coordinateSystem_(static_cast<int>(CoordinateSystem::cRightYup))
	{}
    ~Transform() {}

	//---------- ImGui�p�֐� ----------
	void DrawDebug();
	void Reset();
	//---------- ImGui�p�֐� ----------

public:// Setter,Getter, etc...
    [[nodiscard]] DirectX::XMMATRIX CalcWorld();							//	���[���h�s��Z�o
	[[nodiscard]] DirectX::XMMATRIX CalcWorldMatrix(float scaleFactor);		//	���W�n���|�������[���h�s��Z�o
	[[nodiscard]] DirectX::XMFLOAT3 CalcForward() const;					//	�O�����x�N�g���Z�o
	[[nodiscard]] DirectX::XMFLOAT3 CalcUp() const;							//	������x�N�g���Z�o
	[[nodiscard]] DirectX::XMFLOAT3 CalcRight() const;						//	�E�����x�N�g���Z�o

	void SetWorld(DirectX::XMMATRIX world);

	//---------- �ʒu ( Position ) ----------
	void SetPosition(const DirectX::XMFLOAT3& pos) { position_ = pos; }
	void SetPositionX(const float& posX) { position_.x = posX; }
	void SetPositionY(const float& posY) { position_.y = posY; }
	void SetPositionZ(const float& posZ) { position_.z = posZ; }

	[[nodiscard]] const DirectX::XMFLOAT3& GetPosition() const { return position_; }
	[[nodiscard]] const float GetPositionX() const { return position_.x; }
	[[nodiscard]] const float GetPositionY() const { return position_.y; }
	[[nodiscard]] const float GetPositionZ() const { return position_.z; }

	void AddPosition(const DirectX::XMFLOAT3& pos)
	{
		position_.x += pos.x;
		position_.y += pos.y;
		position_.z += pos.z;
	}
	void AddPositionX(const float posX) { position_.x += posX; }
	void AddPositionY(const float posY) { position_.y += posY; }
	void AddPositionZ(const float posZ) { position_.z += posZ; }

	//---------- �傫�� ( Scale ) ----------
	void SetScale(const DirectX::XMFLOAT3& scl) { scale_ = scl; }

	// ScaleFactor
	[[nodiscard]] float GetScaleFactor() const { return scaleFactor_; }
	void SetScaleFactor(float f) { scaleFactor_ = f; }

	[[nodiscard]] DirectX::XMFLOAT3 GetScale() const { return scale_; }

	//---------- ��] ( Rotation ) ----------
	[[nodiscard]] DirectX::XMFLOAT4 GetRotation() const { return rotation_; }
	[[nodiscard]] const float GetRotationX() const { return rotation_.x; }
	[[nodiscard]] const float GetRotationY() const { return rotation_.y; }
	[[nodiscard]] const float GetRotationZ() const { return rotation_.z; }

	void SetRotation(const DirectX::XMFLOAT4& rot) { rotation_ = rot; }
	void SetRotationX(const float rotX) { rotation_.x = rotX; }
	void SetRotationY(const float rotY) { rotation_.y = rotY; }
	void SetRotationZ(const float rotZ) { rotation_.z = rotZ; }

	void AddRotationX(const float rotX) { rotation_.x += rotX; }
	void AddRotationY(const float rotY) { rotation_.y += rotY; }
	void AddRotationZ(const float rotZ) { rotation_.z += rotZ; }

	//---------- �F ( Color ) ----------
	void SetColor(const DirectX::XMFLOAT4& color) { color_ = color; }
	void SetColorR(const float colorR) { color_.x = colorR; }
	void SetColorG(const float colorG) { color_.y = colorG; }
	void SetColorB(const float colorB) { color_.z = colorB; }
	void SetColorA(const float colorA) { color_.w = colorA; }

	[[nodiscard]] DirectX::XMFLOAT4 GetColor() const { return color_; }
	[[nodiscard]] const float GetColorR() const { return color_.x; }
	[[nodiscard]] const float GetColorG() const { return color_.y; }
	[[nodiscard]] const float GetColorB() const { return color_.z; }
	[[nodiscard]] const float GetColorA() const { return color_.w; }

private:
	DirectX::XMFLOAT3 position_ = { 0,0,0 };
	DirectX::XMFLOAT3 scale_	= { 1,1,1 };
	DirectX::XMFLOAT4 rotation_ = { 0,0,0,1 };
	DirectX::XMFLOAT4 color_	= {1,1,1,1};

	float scaleFactor_ = 1.0f;

	int coordinateSystem_;
	const DirectX::XMFLOAT4X4 CoordinateSystemTransforms_[static_cast<int>(CoordinateSystem::cNone)]
	{
		{
			-1, 0, 0, 0,
			 0, 1, 0, 0,
			 0, 0, 1, 0,
			 0, 0, 0, 1,
		},

		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		},

		{
			1, 0, 0, 0,
			0, 0, 1, 0,
			0, 1, 0, 0,
			0, 0, 0, 1
		},
		{
			1, 0, 0, 0,
			0, 0, 1, 0,
			0, 1, 0, 0,
			0, 0, 0, 1
		}
	};

	const std::string coordinateSystemName_[static_cast<int>(CoordinateSystem::cNone)]
	{
		"Right Hand, Y up",
		"Left Hand, Y up",
		"Right Hand, Z up",
		"Left Hand, Z up"
	};
};

