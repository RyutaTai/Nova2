#include "SceneDemo.h"

#include "../Core/Framework.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Camera.h"
#include "../Graphics/ShadowMap.h"
#include "../Scenes/SceneManager.h"
#include "../Scenes/SceneTitle.h"
#include "../Scenes/SceneLoading.h"
#include "../Input/GamePad.h"
#include "../Resources/EffectManager.h"
#include "../Others/MathHelper.h"
#include "../../Game/EnemyManager.h"
#include "../../Game/GameState.h"
#include "../../Game/UIManager.h"

//	������
void SceneDemo::Initialize()
{
	/* ----- �X�v���C�g������ ----- */
	//sprite_[static_cast<int>(SPRITE_GAME::BACK)] = std::make_unique<Sprite>(L"./Resources/Image/Game.png");

	sprite_[static_cast<int>(SPRITE_GAME::Clear)] = std::make_unique<Sprite>(L"./Resources/Image/Clear.png");
	sprite_[static_cast<int>(SPRITE_GAME::GameOver)] = std::make_unique<Sprite>(L"./Resources/Image/GameOver.png");

	/* ----- UI������ ----- */
	ui_[static_cast<int>(UI_GAME::HpGaugeBack)] = new UI(L"./Resources/Image/HpGaugeBack.png");	//	����������UI�N���X�Ń}�l�[�W���[�ɓo�^�����
	ui_[static_cast<int>(UI_GAME::HpGauge)] = new UI(L"./Resources/Image/HpGauge.png");
	ui_[static_cast<int>(UI_GAME::HpFrame)] = new UI(L"./Resources/Image/HpFrame.png");
	ui_[static_cast<int>(UI_GAME::Instructions)] = new UI(L"./Resources/Image/Instructions.png");
	UIManager::Instance().GetUIFromNum(static_cast<int>(UI_GAME::HpGaugeBack))->GetTransform()->SetPosition(88, 41);
	UIManager::Instance().GetUIFromNum(static_cast<int>(UI_GAME::HpGaugeBack))->SetName("HpGaugeBack");
	UIManager::Instance().GetUIFromNum(static_cast<int>(UI_GAME::HpGauge))->GetTransform()->SetPosition(85, 40);
	UIManager::Instance().GetUIFromNum(static_cast<int>(UI_GAME::HpGauge))->SetName("HpGauge");
	UIManager::Instance().GetUIFromNum(static_cast<int>(UI_GAME::HpFrame))->GetTransform()->SetPosition(0, 0);
	UIManager::Instance().GetUIFromNum(static_cast<int>(UI_GAME::HpFrame))->SetName("HpFrame");
	UIManager::Instance().GetUIFromNum(static_cast<int>(UI_GAME::Instructions))->GetTransform()->SetPosition(24, 250);
	UIManager::Instance().GetUIFromNum(static_cast<int>(UI_GAME::Instructions))->SetName("Instructions");
	UIManager::Instance().Initialize();					//	�o�^���I����Ă��珉��������������

	/* ----- �X�e�[�W������ ----- */
	stage_[0] = std::make_unique<Stage>("./Resources/Model/cybercity-2099-v2/city.gltf");			//	�V�e�B���f��
	//stage_[0] = std::make_unique<Stage>("./Resources/Model/cybercity-2099-v2/city_Join.glb");		//	�V�e�B���f��

	/* ----- �V�[���萔�o�b�t�@ ----- */
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = (sizeof(Graphics::SceneConstants));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	Graphics::Instance().GetDevice()->CreateBuffer(&desc, nullptr, sceneConstantBuffer_.GetAddressOf());

	/* ----- �J���������� ----- */
	Camera::Instance().Initialize();

	/* ----- �v���C���[������ ----- */
	player_ = std::make_unique<Player>();
	//player_ = std::make_unique<Player>("./Resources/Model/free-mixamo-retextured-model/source/model4.fbx", false, 60.0f);
	player_->Initialize();

	/* ----- �G�l�~�[������ ----- */
	//dragonkin_ = std::make_unique<Dragonkin>("./Resources/Model/silver-dragonkin-mir4/source/Silver_Dragonkin/Mon_BlackDragon31_Skeleton2.glb");
	//dragonkin_ = std::make_unique<Dragonkin>("./Resources/Model/silver-dragonkin-mir4/source/Silver_Dragonkin/Mon_BlackDragon31_Skeleton2.fbx");
	//dragonkin_->Initialize();

	//drone_ = std::make_unique<Drone>("./Resources/Model/Drone/Drone.glb");


	//drone_ = new Drone("./Resources/Model/Drone/Drone.glb");
	//drone_->Initialize();

	// --- �e�N�X�`���ǂݍ��� ---
	D3D11_TEXTURE2D_DESC texture2dDesc;
	ID3D11Device* device = Graphics::Instance().GetDevice();

	//LoadTextureFromFile(device, L"./Resources/Model/GltfSample/environments/sunset_jhbcentral_4k/sunset_jhbcentral_4k.dds",
	//	shaderResourceViews_[0].GetAddressOf(), &texture2dDesc);
	//
	//LoadTextureFromFile(device, L"./Resources/Model/GltfSample/environments/sunset_jhbcentral_4k/diffuse_iem.dds",
	//	shaderResourceViews_[1].GetAddressOf(), &texture2dDesc);
	//
	//LoadTextureFromFile(device, L"./Resources/Model/GltfSample/environments/sunset_jhbcentral_4k/specular_pmrem.dds",
	//	shaderResourceViews_[2].GetAddressOf(), &texture2dDesc);
	//
	//LoadTextureFromFile(device, L"./Resources/Model/GltfSample/environments/lut_ggx.dds",
	//	shaderResourceViews_[3].GetAddressOf(), &texture2dDesc);

	//	�u���[��
	framebuffers_[0] = std::make_unique<FrameBuffer>(device, 1280, 720);
	bitBlockTransfer_ = std::make_unique<FullScreenQuad>(device);
	//	BLOOM
	bloomer_ = std::make_unique<Bloom>(device, 1280, 720);
	Graphics::Instance().GetShader()->CreatePsFromCso(device, "./Resources/Shader/FinalPassPs.cso", pixelShaders_[static_cast<int>(PixelShader::Bloom)].ReleaseAndGetAddressOf());

	//	Skymap
	Graphics::Instance().GetShader()->CreatePsFromCso(device, "./Resources/Shader/SkymapPs.cso", pixelShaders_[static_cast<int>(PixelShader::Skymap)].GetAddressOf());
	LoadTextureFromFile(device, L"./Resources/Model/GltfSample/environments/sunset_jhbcentral_4k/sunset_jhbcentral_4k.dds",
		skymap_.GetAddressOf(), &texture2dDesc);

	//	�X�e�[�g�o�^
#if 0
	stateMachine_.reset(new StateMachine<State<SceneDemo>>());
	stateMachine_->RegisterState(new GameState::Wave1State(this));		//	Wave1
	stateMachine_->RegisterState(new GameState::Wave2State(this));		//	Wave2
	stateMachine_->RegisterState(new GameState::Wave3State(this));		//	Wave3
	stateMachine_->RegisterState(new GameState::GameClearState(this));	//	�Q�[���N���A
	stateMachine_->RegisterState(new GameState::GameOverState(this));	//	�Q�[���I�[�o�[
	//	�����X�e�[�g�ݒ�
	stateMachine_->SetState(static_cast<int>(SceneDemoState::Wave1));	//	�����X�e�[�g�Z�b�g
#else
	EnemyManager::Instance().DroneSpawn(1);	//	�h���[������
#endif

	/* ----- �I�[�f�B�I������ ----- */
#if 0
	audioInstance_.Initialize();
	bgm_[static_cast<int>(AUDIO_BGM_GAME::NORMAL)] = std::make_unique<Audio>(audioInstance_.GetXAudio2(), L"./Resources/Audio/BGM/Game.wav");
	bgm_[static_cast<int>(AUDIO_BGM_GAME::NORMAL)]->SetVolume(0.3f);
#else
	
#endif

}

//	���Z�b�g
void SceneDemo::Reset()
{
	/* ----- �J���������� ----- */
	Camera::Instance().Initialize();

}

//	�X�V����
void SceneDemo::Update(const float& elapsedTime)
{
	//	BGM�Đ�
	//bgm_[static_cast<int>(AUDIO_BGM_GAME::NORMAL)]->Play();

	GamePad& gamePad = Input::Instance().GetGamePad();

	/* ----- �J�����X�V���� ----- */
	DirectX::XMFLOAT3 cameraTarget = player_->GetTransform()->GetPosition();
	//cameraTarget.y += player_->GetHeight() / 2.0f;
	Camera::Instance().SetTarget(cameraTarget);
	Camera::Instance().Update(elapsedTime);

	/* ----- �X�e�[�g�}�V���X�V���� ----- */
	if(stateMachine_)stateMachine_->Update(elapsedTime);

	/* ----- �v���C���[�X�V���� ----- */
	player_->Update(elapsedTime);

	/* ----- �G�l�~�[�X�V���� ----- */
	EnemyManager::Instance().Update(elapsedTime);
	//drone_->Update(elapsedTime);

	/* ----- �G�t�F�N�g�X�V���� ----- */
	EffectManager::Instance().Update(elapsedTime);

	/* ----- UI�X�V���� ----- */
	UIManager::Instance().Update(elapsedTime);

	//	�Q�[���N���A�ւ̑J�ڂ�Weve3 State���ōs���Ă���

	//	�Q�[���I�[�o�[
	int playerHp = player_->GetHp();
	if (playerHp <= 10)
	{
		ChangeState(SceneDemoState::GameOver);
	}

	//	�^�C�g���֑J��
	if (changeTitle_)
	{
		SceneManager::Instance().ChangeScene(new SceneTitle);
	}

}

//	�|�[�Y�ɂ���
void SceneDemo::IsPose(bool isPose)
{
	player_->SetIsPose(isPose);


}

//	�E�F�[�u�摜�ǂݍ���
void SceneDemo::LoadWaveSprite(const wchar_t* filename)
{
	sprite_[SPRITE_GAME::WAVE] = std::make_unique<Sprite>(filename);
}

//	Shadow�`��
void SceneDemo::ShadowRender()
{
	stage_[0]->ShadowRender();	//	�V�e�B���f��
}

//	�`�揈��
void SceneDemo::Render()
{
	ID3D11ShaderResourceView* nullSrv[15] = { NULL };
	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();
	deviceContext->PSSetShaderResources(0, 15, nullSrv);

	Camera::Instance().SetPerspectiveFov();

	DirectX::XMMATRIX ViewProjection = Camera::Instance().CalcViewProjectionMatrix();
	DirectX::XMFLOAT4X4 viewProjection;
	DirectX::XMStoreFloat4x4(&viewProjection, ViewProjection);
	Graphics::Instance().SetViewProjection(ViewProjection);
	Graphics::Instance().SetLightDirection(lightDirection_);
	Graphics::Instance().SetCameraPosition({ 0,0,1,0 });

	Graphics::SceneConstants sceneConstants = Graphics::Instance().GetSceneConstant();
	deviceContext->UpdateSubresource(sceneConstantBuffer_.Get(), 0, 0, &sceneConstants, 0, 0);
	deviceContext->VSSetConstantBuffers(1, 1, sceneConstantBuffer_.GetAddressOf());
	deviceContext->PSSetConstantBuffers(1, 1, sceneConstantBuffer_.GetAddressOf());

	/* ----- Skymap ----- */
	{
		// SKYMAP

		ID3D11Buffer* constantBuffer = Graphics::Instance().GetConstantBuffer();
		DirectX::XMStoreFloat4x4(&sceneConstants.invViewProjection_, DirectX::XMMatrixInverse(NULL, ViewProjection));
		deviceContext->UpdateSubresource(sceneConstantBuffer_.Get(), 0, 0, &sceneConstants, 0, 0);
		deviceContext->VSSetConstantBuffers(5, 1, sceneConstantBuffer_.GetAddressOf());
		deviceContext->PSSetConstantBuffers(5, 1, sceneConstantBuffer_.GetAddressOf());

		Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ALPHA);
		Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_OFF_ZW_OFF);
		Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::CULL_NONE);
		bitBlockTransfer_->Blit(deviceContext, skymap_.GetAddressOf(), 5, 1, pixelShaders_[static_cast<int>(PixelShader::Skymap)].Get());

	}

	/* ----- ���f���`�� ----- */
	{
#if 1
		//	�X�e�[�g�ݒ�
		Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::CULL_NONE);
		Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_ON_ZW_ON);
		Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ALPHA);

		//	Shadow�͂��̊֐�����Ȃ���ShadowRender()�ł���Ă�
		{
			//Graphics::Instance().SetLightDirection(ShadowMap::Instance().GetLightDirection());
			DirectX::XMFLOAT4 cameraPosition_ = { Camera::Instance().GetEye().x,Camera::Instance().GetEye().y,Camera::Instance().GetEye().z,1.0f };
			Graphics::Instance().SetCameraPosition(cameraPosition_);

			D3D11_VIEWPORT viewport;
			UINT numViewports{ 1 };
			deviceContext->RSGetViewports(&numViewports, &viewport);

#if 1
			Camera::Instance().SetPerspectiveFov();
			DirectX::XMMATRIX Projection = Camera::Instance().GetProjectionMatrix();

			DirectX::XMVECTOR Eye{ DirectX::XMLoadFloat3(&Camera::Instance().GetEye()) };
			DirectX::XMVECTOR Focus{ DirectX::XMLoadFloat3(&Camera::Instance().GetFocus()) };
			DirectX::XMVECTOR Up{ DirectX::XMLoadFloat3(&Camera::Instance().GetUp()) };
			DirectX::XMMATRIX V{ DirectX::XMMatrixLookAtLH(Eye, Focus, Up) };
#else
			DirectX::XMMATRIX P{ DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), aspectRatio, 0.1f, 100.0f) };
			DirectX::XMVECTOR eye{ DirectX::XMLoadFloat4(&ShadowMap::Instance().GetCameraPosition()) };
			DirectX::XMVECTOR focus{ DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };
			DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
			DirectX::XMMATRIX V{ DirectX::XMMatrixLookAtLH(eye, focus, up) };
#endif
			Graphics::Instance().SetViewProjection(V * Projection);

			ID3D11Buffer* shadowConstantBuffer = ShadowMap::Instance().GetConstantBuffer();
			Graphics::SceneConstants sceneConstant = Graphics::Instance().GetSceneConstant();
			deviceContext->UpdateSubresource(shadowConstantBuffer, 0, 0, &sceneConstant, 0, 0);
			deviceContext->VSSetConstantBuffers(1, 1, &shadowConstantBuffer);
			deviceContext->PSSetConstantBuffers(1, 1, &shadowConstantBuffer);

			// SHADOW : bind shadow map at slot 8
			ID3D11ShaderResourceView* srv = ShadowMap::Instance().GetShaderResourceView();
			deviceContext->PSSetShaderResources(8, 1, &srv);
		}
#endif

		/* ----- ���f���`�� ----- */
		if (bloomer_)
		{
			framebuffers_[0]->Clear(deviceContext);
			framebuffers_[0]->Activate(deviceContext);
		}

		deviceContext->PSSetShaderResources(32, 1, shaderResourceViews_[0].GetAddressOf());
		deviceContext->PSSetShaderResources(33, 1, shaderResourceViews_[1].GetAddressOf());
		deviceContext->PSSetShaderResources(34, 1, shaderResourceViews_[2].GetAddressOf());
		deviceContext->PSSetShaderResources(35, 1, shaderResourceViews_[3].GetAddressOf());

		/* ----- �X�e�[�W ----- */
		//	�X�e�[�g�ݒ�
		Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::CULL_NONE);
		//Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::WIREFRAME);
		Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_ON_ZW_ON);
		Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ALPHA);
		stage_[0]->Render();

		/* ----- �v���C���[ ----- */
		//	�X�e�[�g�ݒ�
		Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::SOLID);
		//Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::WIREFRAME);
		Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_ON_ZW_ON);
		Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ALPHA);
		player_->Render();

		/* ----- �G�l�~�[ ----- */
		//	�X�e�[�g�ݒ�
		Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::SOLID);
		Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_ON_ZW_ON);
		Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ALPHA);
		EnemyManager::Instance().Render();
		//dragonkin_->Render();
		//drone_->Render();

		// BLOOM
		if (bloomer_)
		{
			framebuffers_[0]->Deactivate(deviceContext);
			bloomer_->Make(deviceContext, framebuffers_[0]->shaderResourceViews_[0].Get());

			Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_OFF_ZW_OFF);
			Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::CULL_NONE);
			Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ALPHA);
			ID3D11ShaderResourceView* shaderResourceViews[] =
			{
				framebuffers_[0]->shaderResourceViews_[0].Get(),
				bloomer_->ShaderResourceView(),
			};
			bitBlockTransfer_->Blit(deviceContext, shaderResourceViews, 0, 2, pixelShaders_[static_cast<int>(PixelShader::Bloom)].Get());
		}

	}

	/* ----- �G�t�F�N�g�`�� ----- */
	{
		Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::SOLID);
		Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_ON_ZW_ON);
		Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ALPHA);
#if 1
		DirectX::XMFLOAT4X4 view;
		DirectX::XMStoreFloat4x4(&view, Camera::Instance().GetViewMatrix());
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMStoreFloat4x4(&projection, Camera::Instance().GetProjectionMatrix());
		EffectManager::Instance().Render(view, projection);
#else
		D3D11_VIEWPORT viewport;
		UINT numViewports{ 1 };
		deviceContext->RSGetViewports(&numViewports, &viewport);
		float aspectRatio{ viewport.Width / viewport.Height };
		DirectX::XMMATRIX Projection{ DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60), aspectRatio, 0.1f, 10000.0f) };
		DirectX::XMVECTOR Eye{ DirectX::XMLoadFloat3(&Camera::Instance().GetEye()) };
		DirectX::XMVECTOR Focus{ DirectX::XMLoadFloat3(&Camera::Instance().GetFocus()) };
		DirectX::XMVECTOR Up{ DirectX::XMLoadFloat3(&Camera::Instance().GetUp()) };
		DirectX::XMMATRIX V{ DirectX::XMMatrixLookAtLH(Eye, Focus, Up) };
		DirectX::XMFLOAT4X4 view;
		DirectX::XMStoreFloat4x4(&view, V);
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMStoreFloat4x4(&projection, Projection);
		EffectManager::Instance().Render(view, projection);
#endif
	}

	/* ----- �f�o�b�O�v���~�e�B�u�`�� ----- */
	{
		//	�f�o�b�O�����_���`����s
#if _DEBUG
		player_->DrawDebugPrimitive();
		EnemyManager::Instance().DrawDebugPrimitive();
		Graphics::Instance().GetDebugRenderer()->Render();
#endif
	}

	/* ----- �X�v���C�g�`�� ----- */
	{
		//	��O�ɃX�v���C�g�o���Ȃ�ZON_ON�A���ɕ`��Ȃ�OFF_OFF
		Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::CULL_NONE);//	�e�X�e�[�g���̃X�v���C�g�`��
		Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_ON_ZW_ON);
		//Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_OFF_ZW_OFF);
		Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ALPHA);

		//sprite_[static_cast<int>(SPRITE_GAME::BACK)]->GetTransform()->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		//sprite_[static_cast<int>(SPRITE_GAME::BACK)]->Render();	//	�Q�[���X�v���C�g�`��

		//	�E�F�[�u���`��
		if (sprite_[SPRITE_GAME::WAVE] && waveStartTimer_ > 0)
		{
			sprite_[static_cast<int>(SPRITE_GAME::WAVE)]->Render();
		}

		//	HP�֘A�`��
		//Sprite* hpBack = sprite_[SPRITE_GAME::HpGaugeBack].get();
		//Sprite* hpGauge = sprite_[SPRITE_GAME::HpGauge].get();
		//Sprite* hpFrame = sprite_[SPRITE_GAME::HpFrame].get();

		//hpBack->Render();

		int hp = Player::Instance().GetHp();

		//hpGauge->GetTransform()->SetSizeX(14.24 * hp);

		//hpGauge->GetTransform()->SetSize(1280, 720);	//	�摜�T�C�Y

		//hpGauge->Render();

		//hpFrame->Render();

		//	������@�`��
		if (waveStartTimer_ <= 0.0f && isResult_ == false)
		{
			ui_[static_cast<int>(UI_GAME::Instructions)]->Render();
		}

		//	�Q�[���N���A
		if (isGameClear_)
		{
			sprite_[static_cast<int>(SPRITE_GAME::Clear)]->Render();
		}

		//	�Q�[���I�[�o�[
		if (isGameOver_)
		{
			sprite_[static_cast<int>(SPRITE_GAME::GameOver)]->Render();
		}

	}

	/* ----- UI�`�� ----- */
	UIManager::Instance().Render();
	//if (drawUI_)UIManager::Instance().Render();

}

//	�I����
void SceneDemo::Finalize()
{
	//	�X�v���C�g�I����
	for (int i = 0; i < static_cast<int>(SPRITE_GAME::MAX); i++)
	{
		if (sprite_[i] != nullptr)
		{
			sprite_[i] = nullptr;
		}
	}
	//	�X�e�[�W�I����
	for (int i = 0; i < 2; i++)
	{
		if (stage_[i] != nullptr)
		{
			stage_[i] = nullptr;
		}
	}

	//	UI�I����
	UIManager::Instance().Finalize();

}

//	�f�o�b�O�`��
void SceneDemo::DrawDebug()
{
	D3D11_VIEWPORT viewport;
	UINT numViewports{ 1 };
	Graphics::Instance().GetDeviceContext()->RSGetViewports(&numViewports, &viewport);

	auto srv = ShadowMap::Instance().GetShaderResourceView();
	ImGui::Image(reinterpret_cast<void*>(srv), ImVec2(viewport.Width / 5.0f, viewport.Height / 5.0f));

	//	3D�I�[�f�B�I
	if (ImGui::TreeNode("3DAudio"))
	{
		ImGui::DragFloat3("EmitterPos", &emitterPos_.x, 0.1f, FLT_MIN, FLT_MAX);
		ImGui::DragFloat("SoundRadius", &soundRadius_, 0.1f, FLT_MIN, FLT_MAX);
		ImGui::DragFloat("TestScaler", &testScaler_, 0.1f, FLT_MIN, FLT_MAX);		//	�����X�P�[���l
		ImGui::DragFloat("TestVolume", &testVolume_, 0.1f, FLT_MIN, FLT_MAX);		//	�����X�P�[���l

		if (ImGui::TreeNode("TestListener"))
		{
			ImGui::DragFloat3("TestListenerPos", &testListenerPos_.x, 0.1f, FLT_MIN, FLT_MAX);
			ImGui::DragFloat3("TestListenerFront", &testListenerFront_.x, 0.1f, -90.0f, +90.0f);
			float testListerFrontLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&testListenerFront_)));
			ImGui::DragFloat("TestListenerFrontLength", &testListerFrontLength);
			ImGui::DragFloat3("TestListenerTop", &testListenerTop_.x, 0.1f, -90.0f, +90.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("TestEmitter"))
		{
			ImGui::DragFloat3("TestEmitterPos", &testEmitterPos_.x, 0.1f, FLT_MIN, FLT_MAX);
			ImGui::DragFloat("TestEmitterRadius", &testEmitterRadius_, 0.1f, FLT_MIN, FLT_MAX);
			ImGui::DragFloat("TestEmitterMaxVolumeRadius", &testEmitterMaxVolumeRadius_, 0.1f, FLT_MIN, FLT_MAX);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	ImGui::DragFloat4("LightDirection", &lightDirection_.x, 0.1f, -FLT_MAX, FLT_MAX);	//	���C�g�̌���

	if (bloomer_)bloomer_->DrawDebug();

	Camera::Instance().DrawDebug();		//	Camera
	stage_[0]->DrawDebug();				//	Stage
	ShadowMap::Instance().DrawDebug();	//	Shadow
	player_->DrawDebug();				//	Player

	BulletManager::Instance().DrawDebug();	//	BulletManager
	EnemyManager::Instance().DrawDebug();	//	EnemyManager
	//enemy_->DrawDebug();					//	Enemy
	//dragonkin_->DrawDebug();				//	Dragonkin
	//drone_->DrawDebug();					//	Drone

	UIManager::Instance().DrawDebug();		//	UIManager��UI

	ImGui::Checkbox("DrawUI", &drawUI_);	//	UI�`��؂�ւ�

}