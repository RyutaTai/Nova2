#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

class DebugRenderer
{
public:
	DebugRenderer(ID3D11Device* device);
	~DebugRenderer() {}

public:
	// �`����s
	void Render();

	// ���`��
	void DrawSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color);

	// �~���`��
	void DrawCylinder(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color = { 0,0,0,1 });

	// �~���`��
	void DrawCone(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& axis, float radius, float height, const DirectX::XMFLOAT4& color);

private:
	// �����b�V���쐬
	void CreateSphereMesh(ID3D11Device* device, float radius, int slices, int stacks);

	// �~�����b�V���쐬
	void CreateCylinderMesh(ID3D11Device* device, float radius1, float radius2, float start, float height, int slices, int stacks);

	// �~�����b�V���쐬
	void CreateConeMesh(ID3D11Device* device, float radius, float height, int slices);

private:
	struct CbMesh
	{
		DirectX::XMFLOAT4X4	wvp_;
		DirectX::XMFLOAT4	color_;
	};

	struct Sphere
	{
		DirectX::XMFLOAT4	color_;
		DirectX::XMFLOAT3	center_;
		float				radius_;
	};

	struct Cylinder
	{
		DirectX::XMFLOAT4	color_;
		DirectX::XMFLOAT3	position_;
		float				radius_;
		float				height_;
	};

	struct Cone
	{
		DirectX::XMFLOAT3 position_;
		DirectX::XMFLOAT3 direction_; // �ǉ�
		float radius_;
		float height_;
		DirectX::XMFLOAT4 color_;
	};


	Microsoft::WRL::ComPtr	<ID3D11Buffer>				sphereVertexBuffer_;
	Microsoft::WRL::ComPtr	<ID3D11Buffer>				cylinderVertexBuffer_;
	Microsoft::WRL::ComPtr	<ID3D11Buffer>				coneVertexBuffer_;
	Microsoft::WRL::ComPtr	<ID3D11Buffer>				constantBuffer_;

	Microsoft::WRL::ComPtr	<ID3D11VertexShader>		vertexShader_;
	Microsoft::WRL::ComPtr	<ID3D11PixelShader>			pixelShader_;
	Microsoft::WRL::ComPtr	<ID3D11InputLayout>			inputLayout_;

	Microsoft::WRL::ComPtr	<ID3D11BlendState>			blendState_;
	Microsoft::WRL::ComPtr	<ID3D11RasterizerState>		rasterizerState_;
	Microsoft::WRL::ComPtr	<ID3D11DepthStencilState>	depthStencilState_;

	std::vector	<Sphere>	spheres_;
	std::vector	<Cylinder>	cylinders_;
	std::vector	<Cone>		cones_;

	UINT	sphereVertexCount_ = 0;
	UINT	cylinderVertexCount_ = 0;
	UINT	coneVertexCount_ = 0;

};
