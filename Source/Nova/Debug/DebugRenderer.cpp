#include "DebugRenderer.h"

#include <stdio.h>
#include <memory>

#include "../Others/Misc.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Camera.h"

DebugRenderer::DebugRenderer(ID3D11Device* device)
{
	//	���_�V�F�[�_�[
	{
		//	�t�@�C�����J��
		FILE* fp = nullptr;
		fopen_s(&fp, "./Resources/Shader/DebugVS.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		//	�t�@�C���̃T�C�Y�����߂�
		fseek(fp, 0, SEEK_END);
		long csoSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		//	��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
		std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
		fread(csoData.get(), csoSize, 1, fp);
		fclose(fp);

		// ���_�V�F�[�_�[����
		HRESULT hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vertexShader_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		//	���̓��C�A�E�g
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), csoData.get(), csoSize, inputLayout_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	//	�s�N�Z���V�F�[�_�[
	{
		//	�t�@�C�����J��
		FILE* fp = nullptr;
		fopen_s(&fp, "./Resources/Shader/DebugPS.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		//	�t�@�C���̃T�C�Y�����߂�
		fseek(fp, 0, SEEK_END);
		long csoSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		//	��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
		std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
		fread(csoData.get(), csoSize, 1, fp);
		fclose(fp);

		//	�s�N�Z���V�F�[�_�[����
		HRESULT hr = device->CreatePixelShader(csoData.get(), csoSize, nullptr, pixelShader_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	//	�萔�o�b�t�@
	{
		//	�V�[���p�o�b�t�@
		D3D11_BUFFER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = sizeof(CbMesh);
		desc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&desc, 0, constantBuffer_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	//	�u�����h�X�e�[�g
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = false;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&desc, blendState_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	//	�[�x�X�e���V���X�e�[�g
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	//	���X�^���C�U�[�X�e�[�g
	{
		D3D11_RASTERIZER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = true;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.AntialiasedLineEnable = false;

		HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	//	�����b�V���쐬
	CreateSphereMesh(device, 1.0f, 16, 16);

	//	�~�����b�V���쐬
	CreateCylinderMesh(device, 1.0f, 1.0f, 0.0f, 1.0f, 16, 1);

	// �~�����b�V���쐬
	CreateConeMesh(device, 1.0f, 1.0f, 16);

}

//	�`��
void DebugRenderer::Render()
{
	Graphics& graphics = Graphics::Instance();

	// �V�F�[�_�[�ݒ�
	graphics.GetDeviceContext()->VSSetShader(vertexShader_.Get(), nullptr, 0);
	graphics.GetDeviceContext()->PSSetShader(pixelShader_.Get(), nullptr, 0);
	graphics.GetDeviceContext()->IASetInputLayout(inputLayout_.Get());

	// �萔�o�b�t�@�ݒ�
	graphics.GetDeviceContext()->VSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());

	// �����_�[�X�e�[�g�ݒ�
	const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	graphics.GetDeviceContext()->OMSetBlendState(blendState_.Get(), blendFactor, 0xFFFFFFFF);
	graphics.GetDeviceContext()->OMSetDepthStencilState(depthStencilState_.Get(), 0);
	graphics.GetDeviceContext()->RSSetState(rasterizerState_.Get());

	// �r���[�v���W�F�N�V�����s��쐬
	DirectX::XMMATRIX V = Camera::Instance().GetViewMatrix();
	DirectX::XMMATRIX P = Camera::Instance().GetProjectionMatrix();
	DirectX::XMMATRIX VP = V * P;

	// �v���~�e�B�u�ݒ�
	UINT stride = sizeof(DirectX::XMFLOAT3);
	UINT offset = 0;
	graphics.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// ���`��
	graphics.GetDeviceContext()->IASetVertexBuffers(0, 1, sphereVertexBuffer_.GetAddressOf(), &stride, &offset);
	for (const Sphere& sphere : spheres_)
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(sphere.radius_, sphere.radius_, sphere.radius_);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(sphere.center_.x, sphere.center_.y, sphere.center_.z);
		DirectX::XMMATRIX W = S * T;
		DirectX::XMMATRIX WVP = W * VP;

		CbMesh cbMesh;
		cbMesh.color_ = sphere.color_;
		DirectX::XMStoreFloat4x4(&cbMesh.wvp_, WVP);

		graphics.GetDeviceContext()->UpdateSubresource(constantBuffer_.Get(), 0, 0, &cbMesh, 0, 0);
		graphics.GetDeviceContext()->Draw(sphereVertexCount_, 0);
	}
	spheres_.clear();

	// �~���`��
	graphics.GetDeviceContext()->IASetVertexBuffers(0, 1, cylinderVertexBuffer_.GetAddressOf(), &stride, &offset);
	for (const Cylinder& cylinder : cylinders_)
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(cylinder.radius_, cylinder.height_, cylinder.radius_);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(cylinder.position_.x, cylinder.position_.y, cylinder.position_.z);
		DirectX::XMMATRIX W = S * T;
		DirectX::XMMATRIX WVP = W * VP;

		CbMesh cbMesh;
		cbMesh.color_ = cylinder.color_;
		DirectX::XMStoreFloat4x4(&cbMesh.wvp_, WVP);

		graphics.GetDeviceContext()->UpdateSubresource(constantBuffer_.Get(), 0, 0, &cbMesh, 0, 0);
		graphics.GetDeviceContext()->Draw(cylinderVertexCount_, 0);
	}
	cylinders_.clear();

	// �~���`��
	graphics.GetDeviceContext()->IASetVertexBuffers(0, 1, coneVertexBuffer_.GetAddressOf(), &stride, &offset);
	for (const Cone& cone : cones_)
	{
		// ���[���h�s��̍쐬
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(cone.radius_, cone.height_, cone.radius_);
		DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&cone.direction_);
		dir = DirectX::XMVector3Normalize(dir);

		DirectX::XMMATRIX R;
		if (DirectX::XMVector3Equal(dir, DirectX::XMVectorSet(0, 1, 0, 0))) {
			// �����x�N�g����Y�������ƈ�v����ꍇ�A��]�s��͒P�ʍs��
			R = DirectX::XMMatrixIdentity();
		}
		else if (DirectX::XMVector3Equal(dir, DirectX::XMVectorSet(0, -1, 0, 0))) {
			// �����x�N�g����-Y�������ƈ�v����ꍇ�A��]�s���X����]180�x
			R = DirectX::XMMatrixRotationX(DirectX::XM_PI);
		}
		else {
			// Y������̉�]�s����쐬
			DirectX::XMVECTOR up = DirectX::XMVectorSet(0, 1, 0, 0);
			DirectX::XMVECTOR axis = DirectX::XMVector3Cross(up, dir);
			float angle = acosf(DirectX::XMVectorGetY(dir));
			R = DirectX::XMMatrixRotationAxis(axis, angle);
		}

		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(cone.position_.x, cone.position_.y, cone.position_.z);
		DirectX::XMMATRIX W = S * R * T;
		DirectX::XMMATRIX WVP = W * VP;

		// �萔�o�b�t�@�X�V
		CbMesh cbMesh;
		cbMesh.color_ = cone.color_;
		DirectX::XMStoreFloat4x4(&cbMesh.wvp_, WVP);

		graphics.GetDeviceContext()->UpdateSubresource(constantBuffer_.Get(), 0, 0, &cbMesh, 0, 0);
		graphics.GetDeviceContext()->Draw(coneVertexCount_, 0);
	}
	cones_.clear();
}



//	���`��
void DebugRenderer::DrawSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color)
{
	Sphere sphere;
	sphere.center_ = center;
	sphere.radius_ = radius;
	sphere.color_ = color;
	spheres_.emplace_back(sphere);
}

//	�~���`��
void DebugRenderer::DrawCylinder(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color)
{
	Cylinder cylinder;
	cylinder.position_ = position;
	cylinder.radius_ = radius;
	cylinder.height_ = height;
	cylinder.color_ = color;
	cylinders_.emplace_back(cylinder);
}

//	�~���`��
void DebugRenderer::DrawCone(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, float radius, float height, const DirectX::XMFLOAT4& color)
{
	Cone cone;
	cone.position_ = position;
	cone.direction_ = direction; // �ǉ�
	cone.radius_ = radius;
	cone.height_ = height;
	cone.color_ = color;
	cones_.emplace_back(cone);
}


//	�����b�V���쐬
void DebugRenderer::CreateSphereMesh(ID3D11Device* device, float radius, int slices, int stacks)
{
	sphereVertexCount_ = stacks * slices * 2 + slices * stacks * 2;
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(sphereVertexCount_);

	float phiStep = DirectX::XM_PI / stacks;
	float thetaStep = DirectX::XM_2PI / slices;

	DirectX::XMFLOAT3* p = vertices.get();
	
	for (int i = 0; i < stacks; ++i)
	{
		float phi = i * phiStep;
		float y = radius * cosf(phi);
		float r = radius * sinf(phi);

		for (int j = 0; j < slices; ++j)
		{
			float theta = j * thetaStep;
			p->x = r * sinf(theta);
			p->y = y;
			p->z = r * cosf(theta);
			p++;

			theta += thetaStep;

			p->x = r * sinf(theta);
			p->y = y;
			p->z = r * cosf(theta);
			p++;
		}
	}

	thetaStep = DirectX::XM_2PI / stacks;
	for (int i = 0; i < slices; ++i)
	{
		DirectX::XMMATRIX M = DirectX::XMMatrixRotationY(i * thetaStep);
		for (int j = 0; j < stacks; ++j)
		{
			float theta = j * thetaStep;
			DirectX::XMVECTOR V1 = DirectX::XMVectorSet(radius * sinf(theta), radius * cosf(theta), 0.0f, 1.0f);
			DirectX::XMVECTOR P1 = DirectX::XMVector3TransformCoord(V1, M);
			DirectX::XMStoreFloat3(p++, P1);

			int n = (j + 1) % stacks;
			theta += thetaStep;

			DirectX::XMVECTOR V2 = DirectX::XMVectorSet(radius * sinf(theta), radius * cosf(theta), 0.0f, 1.0f);
			DirectX::XMVECTOR P2 = DirectX::XMVector3TransformCoord(V2, M);
			DirectX::XMStoreFloat3(p++, P2);
		}
	}

	//	���_�o�b�t�@
	{
		D3D11_BUFFER_DESC desc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		desc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * sphereVertexCount_);
		desc.Usage = D3D11_USAGE_IMMUTABLE;	// D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		subresourceData.pSysMem = vertices.get();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, sphereVertexBuffer_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

//	�~�����b�V���쐬
void DebugRenderer::CreateCylinderMesh(ID3D11Device* device, float radius1, float radius2, float start, float height, int slices, int stacks)
{
	cylinderVertexCount_ = 2 * slices * (stacks + 1) + 2 * slices;
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(cylinderVertexCount_);

	DirectX::XMFLOAT3* p = vertices.get();

	float stackHeight = height / stacks;
	float radiusStep = (radius2 - radius1) / stacks;

	//	vertices of ring
	float dTheta = DirectX::XM_2PI / slices;

	for (int i = 0; i < slices; ++i)
	{
		int n = (i + 1) % slices;

		float c1 = cosf(i * dTheta);
		float s1 = sinf(i * dTheta);

		float c2 = cosf(n * dTheta);
		float s2 = sinf(n * dTheta);

		for (int j = 0; j <= stacks; ++j)
		{
			float y = start + j * stackHeight;
			float r = radius1 + j * radiusStep;

			p->x = r * c1;
			p->y = y;
			p->z = r * s1;
			p++;

			p->x = r * c2;
			p->y = y;
			p->z = r * s2;
			p++;
		}

		p->x = radius1 * c1;
		p->y = start;
		p->z = radius1 * s1;
		p++;

		p->x = radius2 * c1;
		p->y = start + height;
		p->z = radius2 * s1;
		p++;
	}

	//	���_�o�b�t�@
	{
		D3D11_BUFFER_DESC desc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		desc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * cylinderVertexCount_);
		desc.Usage = D3D11_USAGE_IMMUTABLE;	// D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		subresourceData.pSysMem = vertices.get();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, cylinderVertexBuffer_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

#if 0
// �~�����b�V���쐬
void DebugRenderer::CreateConeMesh(ID3D11Device* device, float radius, float height, int slices)
{
	coneVertexCount_ = 2 * slices + 2 * slices;
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(coneVertexCount_);

	DirectX::XMFLOAT3* p = vertices.get();

	float dTheta = DirectX::XM_2PI / slices;

	// ���_�v�Z
	for (int i = 0; i < slices; ++i)
	{
		float theta = i * dTheta;
		float nextTheta = (i + 1) % slices * dTheta;

		float x = radius * cosf(theta);
		float z = radius * sinf(theta);

		float nextX = radius * cosf(nextTheta);
		float nextZ = radius * sinf(nextTheta);

		// ���ʂ̒��_
		p->x = x;
		p->y = 0.0f;
		p->z = z;
		p++;

		p->x = 0.0f;
		p->y = height;
		p->z = 0.0f;
		p++;

		// ��ʂ̒��_
		p->x = x;
		p->y = 0.0f;
		p->z = z;
		p++;

		p->x = nextX;
		p->y = 0.0f;
		p->z = nextZ;
		p++;
	}

	// ���_�o�b�t�@
	{
		D3D11_BUFFER_DESC desc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		desc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * coneVertexCount_);
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		subresourceData.pSysMem = vertices.get();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, coneVertexBuffer_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

#else
// �~�����b�V���쐬
void DebugRenderer::CreateConeMesh(ID3D11Device* device, float radius, float height, int slices)
{
	coneVertexCount_ = 2 * slices + 2 * slices;
	std::unique_ptr<DirectX::XMFLOAT3[]> vertices = std::make_unique<DirectX::XMFLOAT3[]>(coneVertexCount_);

	DirectX::XMFLOAT3* p = vertices.get();

	float dTheta = DirectX::XM_2PI / slices;

	// ���_�v�Z
	for (int i = 0; i < slices; ++i)
	{
		float theta = i * dTheta;
		float nextTheta = (i + 1) % slices * dTheta;

		float x = radius * cosf(theta);
		float z = radius * sinf(theta);

		float nextX = radius * cosf(nextTheta);
		float nextZ = radius * sinf(nextTheta);

		// ���ʂ̒��_
		p->x = x;
		p->y = 0.0f;
		p->z = z;
		p++;

		p->x = 0.0f;
		p->y = height;
		p->z = 0.0f;
		p++;

		// ��ʂ̒��_
		p->x = x;
		p->y = 0.0f;
		p->z = z;
		p++;

		p->x = nextX;
		p->y = 0.0f;
		p->z = nextZ;
		p++;
	}

	// ���_�o�b�t�@
	{
		D3D11_BUFFER_DESC desc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		desc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * coneVertexCount_);
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		subresourceData.pSysMem = vertices.get();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&desc, &subresourceData, coneVertexBuffer_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}


#endif