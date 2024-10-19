#include "SceneGame.h"

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
void SceneGame::Initialize()
{
	/* ----- �I�[�f�B�I������ ----- */
	bgm_[static_cast<int>(AUDIO_BGM_GAME::Normal)] = std::unique_ptr<AudioSource>(Audio::Instance().LoadAudioSource("./Resources/Audio/BGM/Game.wav"));
	bgm_[static_cast<int>(AUDIO_BGM_GAME::Normal)]->SetVolume(0.3f, false);

	/* ----- �X�v���C�g������ ----- */
	//sprite_[static_cast<int>(SPRITE_GAME::BACK)] = std::make_unique<Sprite>(Graphics::Instance().GetDevice(), L"./Resources/Image/Game.png");

	sprite_[static_cast<int>(SPRITE_GAME::Clear)]	 = std::make_unique<Sprite>(L"./Resources/Image/Clear.png");
	sprite_[static_cast<int>(SPRITE_GAME::GameOver)] = std::make_unique<Sprite>(L"./Resources/Image/GameOver.png");

	/* ----- UI������(����������UI�N���X�Ń}�l�[�W���[�ɓo�^�����) ----- */
	UIHealth* uiHealth = new UIHealth();
	
	
	ui_[static_cast<int>(UI_GAME::Instructions)] = new UI(L"./Resources/Image/Instructions.png");
	ui_[static_cast<int>(UI_GAME::Instructions)]->GetTransform()->SetPosition(24, 250);
	ui_[static_cast<int>(UI_GAME::Instructions)]->SetName("Instructions");
	ui_[static_cast<int>(UI_GAME::Instructions)]->SetRenderFlag(false);
	UIManager::Instance().Initialize();					//	�o�^���I����Ă��珉��������������(���͉������Ă��Ȃ�)

	/* ----- �X�e�[�W������ ----- */
	//stage_[0] = std::make_unique<Stage>("./Resources/Model/syougiban.glb");
	//stage_[0] = std::make_unique<Stage>("./Resources/Model/cybercity-2099-v2/city_collision_ground2_correct.glb");//	�V�e�B���f��(�n�ʂ̂�)
	stage_[0] = std::make_unique<Stage>("./Resources/Model/cybercity-2099-v2/city.gltf");							//	�V�e�B���f��
	//stage_[0] = std::make_unique<Stage>("./Resources/Model/cybercity-2099-v2/city.fbx", true);					//	�V�e�B���f��

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
	Graphics::Instance().GetShader()->CreatePsFromCso(device, "./Resources/Shader/FinalPassPs.cso", pixelShaders_[0].ReleaseAndGetAddressOf());

	//	�X�e�[�g�o�^
	stateMachine_.reset(new StateMachine<State<SceneGame>>());
	stateMachine_->RegisterState(new GameState::Wave1State(this));		//	Wave1
	stateMachine_->RegisterState(new GameState::Wave2State(this));		//	Wave2
	stateMachine_->RegisterState(new GameState::Wave3State(this));		//	Wave3
	stateMachine_->RegisterState(new GameState::GameClearState(this));	//	�Q�[���N���A
	stateMachine_->RegisterState(new GameState::GameOverState(this));	//	�Q�[���I�[�o�[
	//	�����X�e�[�g�ݒ�
	stateMachine_->SetState(static_cast<int>(SceneGameState::Wave1));	//	�����X�e�[�g�Z�b�g

}

//	���Z�b�g
void SceneGame::Reset()
{
	/* ----- �J���������� ----- */
	Camera::Instance().Initialize();

}

//	�X�V����
void SceneGame::Update(const float& elapsedTime)
{
	//	BGM�Đ�
	bgm_[static_cast<int>(AUDIO_BGM_GAME::Normal)]->Play(true);

	GamePad& gamePad = Input::Instance().GetGamePad();

	/* ----- �J�����X�V���� ----- */
	DirectX::XMFLOAT3 cameraTarget = player_->GetTransform()->GetPosition();
	cameraTarget.y += player_->GetHeight() / 2.0f;
	Camera::Instance().SetTarget(cameraTarget);
	Camera::Instance().Update(elapsedTime);

	/* ----- �X�e�[�g�}�V���X�V���� ----- */
	stateMachine_->Update(elapsedTime);

	/* ----- �v���C���[�X�V���� ----- */
	player_->Update(elapsedTime);

	/* ----- �G�l�~�[�X�V���� ----- */
	EnemyManager::Instance().Update(elapsedTime);
	//drone_->Update(elapsedTime);

	/* ----- �G�t�F�N�g�X�V���� ----- */
	EffectManager::Instance().Update(elapsedTime);

	/* ----- UI�X�V���� ----- */
	UIManager::Instance().Update(elapsedTime);

	//	HP�X�V����

		//	HP�`��
#if 0
	{
		int hp = Player::Instance().GetHp();

		//	TODO:��ŕ`�揈���ł͂Ȃ��X�V�����Ɉڂ�
		//	HP���������Ă���ԁAhpDecreaseTimer������������
		static float hpDecreaseTimer = 1.0f;	//	�^�C�}�[
		float hpDecreaseSpeed = 20.0f;			//	HP�������x(�傫������قǑ����Ȃ�)
		float hpGaugeMaxX = 4.4f;

		float targetSizeX = hpGaugeMaxX * (hp / 2.0f);
		float currentSizeX = ui_[static_cast<int>(UI_GAME::HpGaugeBack)]->GetTransform()->GetSizeX();

		//	hpGaugeBack�̃T�C�Y��hpGauge�̃T�C�Y�ɋ߂Â��悤�ɂ���
		if (currentSizeX > targetSizeX)
		{
			//	�����ŃT�C�Y������������
			currentSizeX -= hpDecreaseSpeed * elapsedTime;

			//	�^�[�Q�b�g�T�C�Y�ȉ��ɍs���߂��Ȃ��悤�ɒ���
			if (currentSizeX < targetSizeX)
			{
				currentSizeX = targetSizeX;
			}
			ui_[static_cast<int>(UI_GAME::HpGaugeBack)]->GetTransform()->SetSizeX(currentSizeX);
		}
		else
		{
			//	���ɃT�C�Y���������ꍇ�A�����Ƀ^�[�Q�b�g�T�C�Y��ݒ�
			ui_[static_cast<int>(UI_GAME::HpGaugeBack)]->GetTransform()->SetSizeX(targetSizeX);
			hpDecreaseTimer = 1.0f;
		}
		ui_[static_cast<int>(UI_GAME::HpGaugeBack)]->Render();

		ui_[static_cast<int>(UI_GAME::HpGauge)]->GetTransform()->SetSizeX(targetSizeX);
		ui_[static_cast<int>(UI_GAME::HpGauge)]->Render();

		ui_[static_cast<int>(UI_GAME::HpFrame)]->Render();
	}

	int hp = Player::Instance().GetHp();
	ui_[static_cast<int>(UI_GAME::HpGauge)]->GetTransform()->SetSizeX(14.24 * hp);

	//hpGauge->GetTransform()->SetSize(1280, 720);	//	�摜�T�C�Y
#endif

	//	�Q�[���N���A�ւ̑J�ڂ�Weve3 State���ōs���Ă���
		
	//	�Q�[���I�[�o�[
	int playerHp = player_->GetHp();
	if (playerHp <= 0)
	{
		ChangeState(SceneGameState::GameOver);
	}

	//	�^�C�g���֑J��
	if (changeTitle_)
	{
		SceneManager::Instance().ChangeScene(new SceneTitle);
	}

}

//	�|�[�Y�ɂ���
void SceneGame::IsPose(bool isPose)
{
	player_->SetIsPose(isPose);


}

//	�E�F�[�u�摜�ǂݍ���
void SceneGame::LoadWaveSprite(const wchar_t* filename)
{
	sprite_[SPRITE_GAME::WAVE] = std::make_unique<Sprite>(filename);
}

//	Shadow�`��
void SceneGame::ShadowRender()
{
	stage_[0]->ShadowRender();	//	�V�e�B���f��
}

//	�`�揈��
void SceneGame::Render()
{
	ID3D11ShaderResourceView* nullSrv[15] = { NULL };
	Graphics::Instance().GetDeviceContext()->PSSetShaderResources(0, 15, nullSrv);

	Camera::Instance().SetPerspectiveFov();

	Graphics::Instance().SetViewProjection(Camera::Instance().CalcViewProjectionMatrix());
	Graphics::Instance().SetLightDirection(lightDirection_);
	Graphics::Instance().SetCameraPosition({ 0,0,1,0 });

	Graphics::SceneConstants sceneConstants = Graphics::Instance().GetSceneConstant();
	Graphics::Instance().GetDeviceContext()->UpdateSubresource(sceneConstantBuffer_.Get(), 0, 0, &sceneConstants, 0, 0);
	Graphics::Instance().GetDeviceContext()->VSSetConstantBuffers(1, 1, sceneConstantBuffer_.GetAddressOf());
	Graphics::Instance().GetDeviceContext()->PSSetConstantBuffers(1, 1, sceneConstantBuffer_.GetAddressOf());

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
			Graphics::Instance().GetDeviceContext()->RSGetViewports(&numViewports, &viewport);

#if 1
			Camera::Instance().SetPerspectiveFov();
			DirectX::XMMATRIX Projection = Camera::Instance().GetProjectionMatrix();
			
			DirectX::XMVECTOR Eye{ DirectX::XMLoadFloat3(&Camera::Instance().GetEye()) };
			DirectX::XMVECTOR Focus{DirectX::XMLoadFloat3(&Camera::Instance().GetFocus()) };
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
			Graphics::Instance().GetDeviceContext()->UpdateSubresource(shadowConstantBuffer, 0, 0, &sceneConstant, 0, 0);
			Graphics::Instance().GetDeviceContext()->VSSetConstantBuffers(1, 1, &shadowConstantBuffer);
			Graphics::Instance().GetDeviceContext()->PSSetConstantBuffers(1, 1, &shadowConstantBuffer);

			// SHADOW : bind shadow map at slot 8
			ID3D11ShaderResourceView* srv = ShadowMap::Instance().GetShaderResourceView();
			Graphics::Instance().GetDeviceContext()->PSSetShaderResources(8, 1, &srv);
		}
#endif

		/* ----- ���f���`�� ----- */

		ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();
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
		Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::CULL_NONE);
		Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_ON_ZW_ON);
		Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ALPHA);
		EnemyManager::Instance().Render();
		//dragonkin_->Render();
		//drone_->Render();

		if (bloomer_)
		{
			framebuffers_[0]->Deactivate(deviceContext);
			// BLOOM
			bloomer_->Make(deviceContext, framebuffers_[0]->shaderResourceViews_[0].Get());

			Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::CULL_NONE);
			Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_OFF_ZW_OFF);
			Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ALPHA);
			ID3D11ShaderResourceView* shaderResourceViews[] =
			{
				framebuffers_[0]->shaderResourceViews_[0].Get(),
				bloomer_->ShaderResourceView(),
			};
			bitBlockTransfer_->Blit(deviceContext, shaderResourceViews, 0, 2, pixelShaders_[0].Get());
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
		Graphics::Instance().GetDeviceContext()->RSGetViewports(&numViewports, &viewport);
		float aspectRatio{ viewport.Width / viewport.Height };
		DirectX::XMMATRIX Projection{ DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60), aspectRatio, 0.1f, 10000.0f) };
		DirectX::XMVECTOR Eye{ DirectX::XMLoadFloat3(&Camera::Instance().GetEye()) };
		DirectX::XMVECTOR Focus{DirectX::XMLoadFloat3(&Camera::Instance().GetFocus()) };
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

		//	������@�`��
		if (waveStartTimer_ <= 0.0f && isResult_ == false)
		{
			ui_[static_cast<int>(UI_GAME::Instructions)]->SetRenderFlag(true);
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

}

//	�I����
void SceneGame::Finalize()
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
void SceneGame::DrawDebug()
{
	D3D11_VIEWPORT viewport;
	UINT numViewports{ 1 };
	Graphics::Instance().GetDeviceContext()->RSGetViewports(&numViewports, &viewport);

	auto srv = ShadowMap::Instance().GetShaderResourceView();
	ImGui::Image(reinterpret_cast<void*>(srv), ImVec2(viewport.Width / 5.0f, viewport.Height / 5.0f));

	ImGui::DragFloat4("LightDirection", &lightDirection_.x, 0.1f, -FLT_MAX, FLT_MAX);	//	���C�g�̌���

	if (bloomer_)bloomer_->DrawDebug();	//	Bloom

	Camera::Instance().DrawDebug();		//	Camera
	
	ShadowMap::Instance().DrawDebug();	//	Shadow
	player_->DrawDebug();				//	Player

	EnemyManager::Instance().DrawDebug();
	//enemy_->DrawDebug();				//	Enemy
	//dragonkin_->DrawDebug();			//	Dragonkin
	//drone_->DrawDebug();				//	Drone
	
	UIManager::Instance().DrawDebug();	//	UIManager��UI

	ImGui::Checkbox("DrawUI", &drawUI_);				//	UI�`��؂�ւ�

}