#include "sprite.h"

#include "../Graphics/Graphics.h"
#include "../../imgui/ImGuiCtrl.h"

//	�R���X�g���N�^
Sprite::Sprite(const wchar_t* fileName)
{
	HRESULT hr = S_OK;
	ID3D11Device* device = Graphics::Instance().GetDevice();

	Vertex vertices[]
	{
		{ { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 0, 0 } },//����
		{ { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 1, 0 } },//�E��
		{ { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 0, 1 } },//����
		{ { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 1, 1 } },//�E��
	};

	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = vertices;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;
	
	hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// �V�F�[�_�[�ǂݍ���
	Shader* shader = Graphics::Instance().GetShader();
	hr = shader->CreateVsFromCso(device, "./Resources/Shader/SpriteVs.cso", vertexShader_.GetAddressOf(), inputLayout_.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));	
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	hr = shader->CreatePsFromCso(device, "./Resources/Shader/SpritePs.cso", pixelShader_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	
	// �e�N�X�`���ǂݍ���
	LoadTextureFromFile(device, fileName, shaderResourceView_.GetAddressOf(), &texture2dDesc_);

	GetTransform()->SetSize(texture2dDesc_.Width, texture2dDesc_.Height);
	GetTransform()->SetTexSize(texture2dDesc_.Width, texture2dDesc_.Height);
}

//	�f�X�g���N�^
Sprite::~Sprite()
{
	
}

//	�`�揈��
void Sprite::Render()
{
	Graphics& graphics = Graphics::Instance();

	D3D11_VIEWPORT viewport{};
	UINT numViewports{ 1 };

	graphics.GetDeviceContext()->RSGetViewports(&numViewports, &viewport);

	//	left-top
	float x0{ GetTransform()->GetPositionX() };
	float y0{ GetTransform()->GetPositionY() };
	//	right-top
	float x1{ GetTransform()->GetPositionX() + GetTransform()->GetSizeX() };
	float y1{ GetTransform()->GetPositionY() };
	//	left-bottom
	float x2{ GetTransform()->GetPositionX() };
	float y2{ GetTransform()->GetPositionY() + GetTransform()->GetSizeY() };
	//	right-bottom
	float x3{ GetTransform()->GetPositionX() + GetTransform()->GetSizeX() };
	float y3{ GetTransform()->GetPositionY() + GetTransform()->GetSizeY() };

	auto rotate = [](float& x, float& y, float cx, float cy, float angle)
	{
		x -= cx;
		y -= cy;

		float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
		float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
		float tx{ x }, ty{ y };
		x = cos * tx + -sin * ty;
		y = sin * tx + cos * ty;

		x += cx;
		y += cy;
	};

#if 1	//	��]�̒��S����`�̒��S�ɂ���ꍇ
	float cx = GetTransform()->GetPositionX() + GetTransform()->GetSizeX() * 0.5f;
	float cy = GetTransform()->GetPositionY() + GetTransform()->GetSizeY() * 0.5f;



#else	//	��]�̒��S������ɂ���ꍇ
	float cx = GetTransform()->GetPositionX();
	float cy = GetTransform()->GetPositionY();
#endif

	rotate(x0, y0, cx, cy, GetTransform()->GetAngle());
	rotate(x1, y1, cx, cy, GetTransform()->GetAngle());
	rotate(x2, y2, cx, cy, GetTransform()->GetAngle());
	rotate(x3, y3, cx, cy, GetTransform()->GetAngle());

	//	screen space to NDC
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;

	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = graphics.GetDeviceContext()->Map(vertexBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	Vertex* vertices{ reinterpret_cast<Vertex*>(mappedSubresource.pData) };
	if (vertices != nullptr)
	{
		vertices[0].position_ = { x0,y0,0 };
		vertices[1].position_ = { x1,y1,0 };
		vertices[2].position_ = { x2,y2,0 };
		vertices[3].position_ = { x3,y3,0 };
		vertices[0].color_ = vertices[1].color_ = vertices[2].color_ = vertices[3].color_ = GetTransform()->GetColor();

		float sx = GetTransform()->GetTexPosX();
		float sy = GetTransform()->GetTexPosY();
		float sw = GetTransform()->GetTexSizeX();
		float sh = GetTransform()->GetTexSizeY();

		vertices[0].texcoord_ = { sx / texture2dDesc_.Width,		sy / texture2dDesc_.Height };
		vertices[1].texcoord_ = { (sx + sw) / texture2dDesc_.Width, sy / texture2dDesc_.Height };
		vertices[2].texcoord_ = { sx / texture2dDesc_.Width,		(sy + sh) / texture2dDesc_.Height };
		vertices[3].texcoord_ = { (sx + sw) / texture2dDesc_.Width, (sy + sh) / texture2dDesc_.Height };
		
	}
	graphics.GetDeviceContext()->Unmap(vertexBuffer_.Get(), 0);

	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };

	graphics.GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);
	graphics.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	graphics.GetDeviceContext()->IASetInputLayout(inputLayout_.Get());
	graphics.GetDeviceContext()->VSSetShader(vertexShader_.Get(), nullptr, 0);
	graphics.GetDeviceContext()->PSSetShader(pixelShader_.Get(), nullptr, 0);

	graphics.GetDeviceContext()->PSSetShaderResources(0, 1, shaderResourceView_.GetAddressOf());

	// �e��X�e�[�g�̐ݒ�
	//Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_ON_ZW_ON);
	//Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ALPHA);
	//Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::CULL_NONE);

	// --- �`�� (�����艺�ɉ��������Ȃ�) ---
	graphics.GetDeviceContext()->Draw(4, 0);
}

//	�e�L�X�g�`��
void Sprite::Textout(std::string s,
	float x, float y, float w, float h, float r, float g, float b, float a)
{
	float sw = static_cast<float>(texture2dDesc_.Width / 16);
	float sh = static_cast<float>(texture2dDesc_.Height / 16);
	float carriage = 0;
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	for (const char c : s)
	{
		GetTransform()->SetPosition(x + carriage, y);
		GetTransform()->SetSize(w, h);
		GetTransform()->SetTexPos(sw * (c & 0x0F), sh * (c >> 4));
		GetTransform()->SetTexSize(sw, sh);
		GetTransform()->SetColor(r, g, b, a);
		Render();
		carriage += w;
	}
}

//void Sprite::Render(ID3D11DeviceContext* deviceContext,
//	float dx, float dy,
//	float dw, float dh,
//	float sx, float sy,
//	float sw, float sh,
//	float angle,
//	float r, float g, float b, float a) const
//{
//	{
//		// ���ݐݒ肳��Ă���r���[�|�[�g����X�N���[���T�C�Y���擾����B
//		D3D11_VIEWPORT viewport;
//		UINT numViewports = 1;
//		deviceContext->RSGetViewports(&numViewports, &viewport);
//		float screen_width = viewport.Width;
//		float screen_height = viewport.Height;
//
//		// �X�v���C�g���\������S���_�̃X�N���[�����W���v�Z����
//		DirectX::XMFLOAT2 positions[] = {
//			DirectX::XMFLOAT2(dx,      dy),			// ����
//			DirectX::XMFLOAT2(dx + dw, dy),			// �E��
//			DirectX::XMFLOAT2(dx,      dy + dh),	// ����
//			DirectX::XMFLOAT2(dx + dw, dy + dh),	// �E��
//		};
//
//		// �X�v���C�g���\������S���_�̃e�N�X�`�����W���v�Z����
//		DirectX::XMFLOAT2 texcoords[] = {
//			DirectX::XMFLOAT2(sx,      sy),			// ����
//			DirectX::XMFLOAT2(sx + sw, sy),			// �E��
//			DirectX::XMFLOAT2(sx,      sy + sh),	// ����
//			DirectX::XMFLOAT2(sx + sw, sy + sh),	// �E��
//		};
//
//		// �X�v���C�g�̒��S�ŉ�]�����邽�߂ɂS���_�̒��S�ʒu��
//		// ���_(0, 0)�ɂȂ�悤�Ɉ�U���_���ړ�������B
//		float mx = dx + dw * 0.5f;
//		float my = dy + dh * 0.5f;
//		for (auto& p : positions)
//		{
//			p.x -= mx;
//			p.y -= my;
//		}
//
//		// ���_����]������
//		const float PI = 3.141592653589793f;
//		float theta = angle * (PI / 180.0f);	// �p�x�����W�A��(��)�ɕϊ�
//		float c = cosf(theta);
//		float s = sinf(theta);
//		for (auto& p : positions)
//		{
//			DirectX::XMFLOAT2 r = p;
//			p.x = c * r.x + -s * r.y;
//			p.y = s * r.x + c * r.y;
//		}
//
//		// ��]�̂��߂Ɉړ����������_�����̈ʒu�ɖ߂�
//		for (auto& p : positions)
//		{
//			p.x += mx;
//			p.y += my;
//		}
//
//		// �X�N���[�����W�n����NDC���W�n�֕ϊ�����B
//		for (auto& p : positions)
//		{
//			p.x = 2.0f * p.x / screen_width - 1.0f;
//			p.y = 1.0f - 2.0f * p.y / screen_height;
//		}
//
//		// ���_�o�b�t�@�̓��e�̕ҏW���J�n����B
//		D3D11_MAPPED_SUBRESOURCE mappedBuffer;
//		HRESULT hr = deviceContext->Map(vertexBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
//		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
//
//		// pData��ҏW���邱�ƂŒ��_�f�[�^�̓��e�����������邱�Ƃ��ł���B
//		Vertex* v = static_cast<Vertex*>(mappedBuffer.pData);
//		for (int i = 0; i < 4; ++i)
//		{
//			v[i].position_.x = positions[i].x;
//			v[i].position_.y = positions[i].y;
//			v[i].position_.z = 0.0f;
//
//			v[i].color_.x = r;
//			v[i].color_.y = g;
//			v[i].color_.z = b;
//			v[i].color_.w = a;
//
//			v[i].texcoord_.x = texcoords[i].x / texture2dDesc_.Width;
//			v[i].texcoord_.y = texcoords[i].y / texture2dDesc_.Height;
//		}
//
//		// ���_�o�b�t�@�̓��e�̕ҏW���I������B
//		deviceContext->Unmap(vertexBuffer_.Get(), 0);
//	}
//
//	{
//		// �p�C�v���C���ݒ�
//		UINT stride = sizeof(Vertex);
//		UINT offset = 0;
//		deviceContext->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);
//		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//		deviceContext->IASetInputLayout(inputLayout_.Get());
//
//		//deviceContext->RSSetState(rasterizerState_.Get());
//
//		deviceContext->VSSetShader(vertexShader_.Get(), nullptr, 0);
//		deviceContext->PSSetShader(pixelShader_.Get(), nullptr, 0);
//
//		deviceContext->PSSetShaderResources(0, 1, shaderResourceView_.GetAddressOf());
//		//deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
//
//		// �`��
//		deviceContext->Draw(4, 0);
//	}
//}


//	�f�o�b�O�`��
void Sprite::DrawDebug()
{
	GetTransform()->DrawDebug();
}

//	�f�o�b�O�`��@
void Sprite::SpriteTransform::DrawDebug()
{
#if USE_IMGUI
	//if (ImGui::TreeNode("Sprite"))
	//{
		ImGui::DragFloat2("position", &position_.x);
		ImGui::DragFloat2("size", &size_.x);
		ImGui::DragFloat2("texPos", &texPos_.x);
		ImGui::DragFloat2("texSize", &texSize_.x);
		
		ImGui::Checkbox("IsDebugSize", &isDebugSize_);
		if (isDebugSize_)
		{
			ImGui::DragFloat("debugSize", &debugSize_);

			size_.x += debugSize_;
			texSize_.x += debugSize_;

			if (fabs(debugSize_) > 0)
				debugSize_ = 0;
		}
		//ImGui::TreePop();
	//}
#endif
}