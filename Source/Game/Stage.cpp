#include "Stage.h"

#include <algorithm>

#include "../Nova/Graphics/Graphics.h"
#include "../Nova/Audio/AudioManager.h"
#include "../Nova/Resources/Texture.h"

Stage* Stage::instance_ = nullptr;

Stage::Stage()
{
	//	インスタンスポイント設定
	_ASSERT_EXPR(instance_ == instance_, L"already instance");
	instance_ = this;

	//	コリジョンメッシュ生成
#if 0
	//collisionMesh_ = std::make_unique<decltype(collisionMesh_)::element_type>(Graphics::Instance().GetDevice(), "./Resources/Model/syougiban.glb");
	collisionMesh_ = std::make_unique<decltype(collisionMesh_)::element_type>(Graphics::Instance().GetDevice(), "./Resources/Model/cybercity-2099-v2/city_collision_ground2_correct.glb");
#else
	
	gltfStaticModelResource_ = ResourceManager::Instance().LoadGltfModelStaticResource("./Resources/Model/cybercity-2099-v2/city.gltf");

	GetTransform()->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	GetTransform()->SetScaleFactor(0.0025f);	//	シティモデル

	collisionMesh_ = std::make_unique<decltype(collisionMesh_)::element_type>(Graphics::Instance().GetDevice(), "./Resources/Model/cybercity-2099-v2/city.gltf");

	//DirectX::XMFLOAT4X4 transform = {};
	//DirectX::XMStoreFloat4x4(&transform, GetTransform()->CalcWorldMatrix(GetTransform()->GetScaleFactor()));
	//collisionMesh_->Transform(transform);
#endif

	//GetTransform()->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	//	エミッシブ定数バッファ生成
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(EmissiveConstants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HRESULT hr;
	hr = Graphics::Instance().GetDevice()->CreateBuffer(&bufferDesc, nullptr, emissiveConstantBuffer_.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	//	midi生成
	midi_ = std::make_unique<Midi>("./Resources/Audio/MIDI/fourOnTheFloor.mid", 8.0f);

	//	音の周波数データ生成、初期化
	frequency_ = std::make_unique<Frequency>();
	frequency_->Initialize();

#if MAGIC_CIRCLE
	D3D11_TEXTURE2D_DESC texture2dDesc;
	LoadTextureFromFile(Graphics::Instance().GetDevice(), L"./Resources/Image/magic circle.png", projectionMappingTexture_.GetAddressOf(), &texture2dDesc);
#endif

	//	スペクトラム用フレームバッファ
	bitBlockTransfer_ = std::make_unique<FullScreenQuad>(Graphics::Instance().GetDevice());
	spectrumFramebuffer_ = std::make_unique<FrameBuffer>(Graphics::Instance().GetDevice(), SPECTRUM_WIDTH, SPECTRUM_HEIGHT);
	Graphics::Instance().GetShader()->CreatePsFromCso(Graphics::Instance().GetDevice(), "./Resources/Shader/SpectrumPS.cso", spectrumPS_.GetAddressOf());

}

//	インスタンス取得
Stage& Stage::Instance()
{
	return *instance_;
}

//	更新処理
void Stage::Update(const float& elapsedTime)
{
	//	midi更新処理
	midi_->Update(elapsedTime);

	//	エミッシブ更新処理
	UpdateEmissive(elapsedTime);

}

//	エミッシブ更新処理
void Stage::UpdateEmissive(const float& elapsedTime)
{
#if 1	//	通常
	//	frequencyを使用しないならemissiveIntensityを1.0fに設定
	if (useFrequency_ == false)emissiveConstant_.emissiveIntensity_ = 1.0f;

	//	周波数データ更新
	frequency_->Update(elapsedTime, AudioManager::Instance().GetAudioResource("Game.wav"));
	//frequency_->Update(elapsedTime, AudioManager::Instance().GetAudioResource("fourOnTheFloor_Basic_44100Hz_16bit.wav"));

	//	frequencyData_更新(配列のデータをずらし、新しいデータを設定)
	for (int i = FrequencyDataMax - 1; 0 < i; --i)
	{
		frequencyData_[i] = frequencyData_[i - 1];
	}
	currentFrequencyValue_ = frequency_->GetAmplitudeSpectrum(frequencyIndex_);
	frequencyData_[0] = currentFrequencyValue_;

	//	frequencyの最大値と最小値を更新
	UpdateFrequencyMin();
	UpdateFrequencyMax();

	//	フーリエ変換で取得した振幅
	float frequencyValue = currentFrequencyValue_;
	if (frequencyMaxValue_ > 0.0f)frequencyValue = (frequencyValue - frequencyMinValue_) / frequencyMaxValue_;

	//	BPM取得
	//float bpm = AudioManager::Instance().GetAudioResource("Game.wav")->GetWaveFormat().GetBPM();

	//	emissiveIntensity_更新
	emissiveConstant_.emissiveIntensity_ = frequencyValue * emissiveFactor_;

#else	//	currentFrequencyValue_にcurrent閾値を設定し、それを越していないならエミッシブにデフォルト値を設定してreturnする

	//	frequencyを使用しないならemissiveIntensityを1.0fに設定
	if (useFrequency_ == false)emissiveConstant_.emissiveIntensity_ = 1.0f;

	//	周波数データ更新
	frequency_->Update(elapsedTime, AudioManager::Instance().GetAudioResource("Game.wav"));
	//frequency_->Update(elapsedTime, AudioManager::Instance().GetAudioResource("fourOnTheFloor_Basic_44100Hz_16bit.wav"));

	//	frequencyData_更新(配列のデータをずらし、新しいデータを設定)
	for (int i = FrequencyDataMax - 1; 0 < i; --i)
	{
		frequencyData_[i] = frequencyData_[i - 1];
	}
	currentFrequencyValue_ = frequency_->GetAmplitudeSpectrum(frequencyIndex_);

	//	閾値に達していなければデフォルト値を設定してreturn
	if (currentFrequencyValue_ < threshold_)
	{
		emissiveConstant_.emissiveIntensity_ = defaultEmissiveIntensity_;
		return;
	}

	frequencyData_[0] = currentFrequencyValue_;

	//	frequencyの最大値と最小値を更新
	UpdateFrequencyMin();
	UpdateFrequencyMax();

	//	フーリエ変換で取得した振幅
	float frequencyValue = currentFrequencyValue_;
	if (frequencyMaxValue_ > 0.0f)frequencyValue = (frequencyValue - frequencyMinValue_) / frequencyMaxValue_;

	//	BPM取得
	//float bpm = AudioManager::Instance().GetAudioResource("Game.wav")->GetWaveFormat().GetBPM();

	//	emissiveIntensity_更新
	emissiveConstant_.emissiveIntensity_ = frequencyValue * emissiveFactor_;
	/*emissiveConstant_.emissiveIntensity_ = std::clamp(currentFrequencyValue,
		emissiveIntencityMin_, emissiveIntencityMax_);*/

#endif
}

//	振幅最小値更新処理
void Stage::UpdateFrequencyMin()
{
	frequencyMinValue_ = FLT_MAX;				//	周波数の最小値
	for (int i = 0; i < FrequencyDataMax; ++i)
	{
		if (frequencyData_[i] < frequencyMinValue_)	//	最小値更新
		{
			frequencyMinValue_ = frequencyData_[i];
		}
	}
}

//	振幅最大値更新処理
void Stage::UpdateFrequencyMax()
{
	frequencyMaxValue_ = FLT_MIN;
	for (int i = 0; i < FrequencyDataMax; ++i)
	{
		float frequency = frequencyData_[i] - frequencyMinValue_;
		if (frequencyMaxValue_ < frequency)	//	最大値更新
		{
			frequencyMaxValue_ = frequency;
		}
	}
}

float Stage::CalculateAutocorrelation(const float data[], const int& lag)
{
	float result = 0.0f;

	for (int i = 0; i < FrequencyDataMax - lag; ++i)
	{
		result += data[i] * data[i + lag];
	}

	return result / (FrequencyDataMax - lag);
}

//	コリジョンメッシュの当たり判定
bool Stage::Collision(_In_ const DirectX::XMFLOAT3& rayStartPosition, _In_ const DirectX::XMFLOAT3& rayDirection, _In_ const DirectX::XMFLOAT4X4& stageTransform, _Out_ DirectX::XMFLOAT3& intersectionPosition, _Out_ DirectX::XMFLOAT3& intersectionNormal,
	_Out_ std::string& intersectionMesh, _Out_ std::string& intersectionMaterial, _In_ float rayLengthLimit, _In_ bool skipIf) const
{
#if 0
	if (collisionMesh_->Raycast(rayStartPosition, rayDirection, transform, intersectionPosition, intersectionNormal, intersectionMesh, intersectionMaterial, rayLengthLimit, skipIf))
	{
		OutputDebugStringA("Position:");
		OutputDebugStringA("Intersected : ");
		OutputDebugStringA(intersectionMesh.c_str());
		OutputDebugStringA(" : ");
		OutputDebugStringA(intersectionMaterial.c_str());
		OutputDebugStringA("\n");
		return true;
	}
	else
	{
		OutputDebugStringA("Unintersected...\n");
		return false;
	}
#else
	//	空間分割
	if (collisionMesh_->RaycastWithSpaceDivision(rayStartPosition, rayDirection, stageTransform, intersectionPosition, intersectionNormal, intersectionMesh, intersectionMaterial, rayLengthLimit, skipIf))
	{
		OutputDebugStringA("Position:");
		OutputDebugStringA("Intersected : ");
		OutputDebugStringA(intersectionMesh.c_str());
		OutputDebugStringA(" : ");
		OutputDebugStringA(intersectionMaterial.c_str());
		OutputDebugStringA("\n");
		return true;
	}
	else
	{
		OutputDebugStringA("Unintersected...\n");
		return false;
	}
#endif
}

//	Shadow描画
void Stage::ShadowRender(const float& scale)
{
#if 0
	ID3D11RenderTargetView* null_render_target_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	Graphics::Instance().GetDeviceContext()->OMSetRenderTargets(_countof(null_render_target_views), null_render_target_views, 0);
	ID3D11ShaderResourceView* null_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	Graphics::Instance().GetDeviceContext()->VSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);
	Graphics::Instance().GetDeviceContext()->PSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);
#endif
	ID3D11PixelShader* nullPixelShader{ NULL };

	//gltfStaticModelResource_->Render(GetTransform()->CalcWorldMatrix(scale));
	//gltfStaticModelResource_->Render(GetTransform()->CalcWorldMatrix(scale), GetTransform()->GetColor(), nullptr, &nullPixelShader);

}

//	描画処理
void Stage::Render()
{
	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();
	// PROJECTION_MAPPING
#if MAGIC_CIRCLE
	Graphics::Instance().GetDeviceContext()->PSSetShaderResources(15, 1, projectionMappingTexture_.GetAddressOf());
#else
	CreateProjectionMappingTextureFromFFT();	//	プロジェクションマッピング用のテクスチャを作成し、シェーダーリソースにセット
	spectrumFramebuffer_->Clear(deviceContext, 0, 0, 0, 1);
	spectrumFramebuffer_->Activate(deviceContext);
	bitBlockTransfer_->Blit(deviceContext, projectionMappingTexture_.GetAddressOf(), 15, 0, spectrumPS_.Get());
	spectrumFramebuffer_->Deactivate(deviceContext);
	//spectrumFramebuffer_->shaderResourceViews_[0].GetAddressOf()
	Graphics::Instance().GetDeviceContext()->PSSetShaderResources(15, 1, spectrumFramebuffer_->shaderResourceViews_[0].GetAddressOf());
#endif

	//	エミッシブ定数バッファをGPUに送る
	deviceContext->UpdateSubresource(emissiveConstantBuffer_.Get(), 0, 0, &emissiveConstant_, 0, 0);
	deviceContext->PSSetConstantBuffers(3, 1, emissiveConstantBuffer_.GetAddressOf());

	//	ピクセルシェーダーセッ
	gltfStaticModelResource_->SetPixelShaderFromName("./Resources/Shader/CityPS.cso");

	gltfStaticModelResource_->Render();		//	描画

}

//	FFTのデータからプロジェクションマッピング用のテクスチャを生成し、セットする
void Stage::CreateProjectionMappingTextureFromFFT()
{
	std::vector<float> fftData = frequency_->GetAmplitudeSpectrum(); // FFT結果を取得

#if 0	//	正規化
	float max = -1.0f;
	for (auto& fft : fftData)
	{
		if (max < fft)max = fft;
	}

	for (auto& fft : fftData)
	{
		fft /= max;
	}
#else
	for (auto& fft : fftData)
	{
		fft /= 100000;
	}
#endif
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = fftData.data();
	initData.SysMemPitch = sizeof(float) * fftData.size();

	//	FFTデータをもとにテクスチャを作成
	D3D11_TEXTURE2D_DESC texture2dDesc = {};
#if MAGIC_CIRCLE
	texture2dDesc.Width = fftData.size();
	texture2dDesc.Height = 1; // 1行のテクスチャとして表現
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R32_FLOAT;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.Usage = D3D11_USAGE_DYNAMIC;
	texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> fftTexture;
	Graphics::Instance().GetDevice()->CreateTexture2D(&texture2dDesc, &initData, fftTexture.GetAddressOf());

	//	テクスチャをシェーダーリソースにバインド(スロット番号 : 15)
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texture2dDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1; WARNING_IPSEC_MM_POLICY_PRUNED;

	Graphics::Instance().GetDevice()->CreateShaderResourceView(fftTexture.Get(), &srvDesc, fftSRV_.GetAddressOf());
	Graphics::Instance().GetDeviceContext()->PSSetShaderResources(15, 1, fftSRV_.GetAddressOf());
#else
	texture2dDesc.Width = fftData.size();
	texture2dDesc.Height = 1; // 1行のテクスチャとして表現
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R32_FLOAT;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.Usage = D3D11_USAGE_DYNAMIC;
	texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> fftTexture;
	Graphics::Instance().GetDevice()->CreateTexture2D(&texture2dDesc, &initData, fftTexture.GetAddressOf());

	//	テクスチャをシェーダーリソースにバインド(スロット番号 : 16)
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texture2dDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1; WARNING_IPSEC_MM_POLICY_PRUNED;

	Graphics::Instance().GetDevice()->CreateShaderResourceView(fftTexture.Get(), &srvDesc, fftSRV_.GetAddressOf());
	Graphics::Instance().GetDeviceContext()->PSSetShaderResources(16, 1, fftSRV_.GetAddressOf());
#endif

}

//	デバッグ描画
void Stage::DrawDebug()
{
	if (ImGui::TreeNode(u8"Stageステージ"))
	{
		//	FFTデータから生成したテクスチャ
		if (ImGui::TreeNode(u8"fftSRV"))
		{
			D3D11_VIEWPORT viewport;
			UINT numViewports{ 1 };
			Graphics::Instance().GetDeviceContext()->RSGetViewports(&numViewports, &viewport);
			auto srv = fftSRV_.Get();
			ImGui::Image(reinterpret_cast<void*>(srv), ImVec2(viewport.Width / 5.0f, viewport.Height / 5.0f));
			ImGui::TreePop();
		}

		//	周波数データのデバッグ描画
		if (ImGui::TreeNode("Frequency Data"))
		{
			frequency_->DrawDebug();
			ImGui::Checkbox("UseFrequency", &useFrequency_);
			ImGui::DragInt("FrequencyIndex", &frequencyIndex_, 1.0f, 0);
			ImGui::DragFloat("CurrentFrequency", &currentFrequencyValue_, 1.0f, 0.0f);
			ImGui::DragFloat("FrequencyMin", &frequencyMinValue_, 1.0f, 0.0f);
			ImGui::DragFloat("FrequencyMax", &frequencyMaxValue_, 1.0f, 0.0f);
			ImGui::DragFloat("EmissiveIntencity", &emissiveConstant_.emissiveIntensity_, 0.1f, 0.0f, FLT_MAX);
			ImGui::DragFloat("DefaultEmissiveIntencity", &defaultEmissiveIntensity_, 0.1f, 0.0f, FLT_MAX);
			ImGui::DragFloat("EmissiveThreshold", &threshold_, 0.1f, 0.0f, FLT_MAX);
			ImGui::DragFloat("EmissiveFactor", &emissiveFactor_, 1.0f, 0.0f);
			ImGui::DragFloat("EmissiveIntencityMin", &emissiveIntencityMin_, 1.0f, 0.0f);
			ImGui::DragFloat("EmissiveIntencityMax", &emissiveIntencityMax_, 1.0f, 0.0f);
			ImGui::TreePop();
		}

		gltfStaticModelResource_->DrawDebug();
		GetTransform()->DrawDebug();
		ImGui::TreePop();
	}
}
