#include "Stage.h"

#include <algorithm>

#include "../Nova/Graphics/Graphics.h"
#include "../Nova/Audio/AudioManager.h"

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
	EmissiveUpdate(elapsedTime);

}

//	エミッシブ更新処理
void Stage::EmissiveUpdate(const float& elapsedTime)
{
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

	frequencyMinValue_ = FLT_MAX;				//	周波数の最小値
	for (int i = 0; i < FrequencyDataMax; ++i)
	{
		if (frequencyData_[i] < frequencyMinValue_)	//	最小値更新
		{
			frequencyMinValue_ = frequencyData_[i];
		}
	}

	frequencyMaxValue_ = FLT_MIN;
	for (int i = 0; i < FrequencyDataMax; ++i)
	{
		float frequency = frequencyData_[i] - frequencyMinValue_;
		if (frequencyMaxValue_ < frequency)	//	最大値更新
		{
			frequencyMaxValue_ = frequency;
		}
	}

	//	フーリエ変換で取得した振幅
	float frequencyValue = currentFrequencyValue_;
	if (frequencyMaxValue_ > 0.0f)frequencyValue = (frequencyValue - frequencyMinValue_) / frequencyMaxValue_;

	//	BPM取得
	//float bpm = AudioManager::Instance().GetAudioResource("Game.wav")->GetWaveFormat().GetBPM();

	//	emissiveIntensity_更新
	emissiveConstant_.emissiveIntensity_ = frequencyValue * emissiveFactor_;
	/*emissiveConstant_.emissiveIntensity_ = std::clamp(currentFrequencyValue,
		emissiveIntencityMin_, emissiveIntencityMax_);*/
}

//	コリジョンメッシュの当たり判定
bool Stage::Collision(_In_ DirectX::XMFLOAT3 rayStartPosition, _In_ DirectX::XMFLOAT3 rayDirection, _In_ const DirectX::XMFLOAT4X4& transform, _Out_ DirectX::XMFLOAT3& intersectionPosition, _Out_ DirectX::XMFLOAT3& intersectionNormal,
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
	if (collisionMesh_->RaycastWithSpaceDivision(rayStartPosition, rayDirection, transform, intersectionPosition, intersectionNormal, intersectionMesh, intersectionMaterial, rayLengthLimit, skipIf))
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
	//	エミッシブ定数バッファをGPUに送る
	Graphics::Instance().GetDeviceContext()->UpdateSubresource(emissiveConstantBuffer_.Get(), 0, 0, &emissiveConstant_, 0, 0);
	Graphics::Instance().GetDeviceContext()->PSSetConstantBuffers(3, 1, emissiveConstantBuffer_.GetAddressOf());

	//	ピクセルシェーダーセット
	gltfStaticModelResource_->SetPixelShaderFromName("./Resources/Shader/CityPS.cso");

	gltfStaticModelResource_->Render();		//	描画

}

//	デバッグ描画
void Stage::DrawDebug()
{
	if (ImGui::TreeNode(u8"Stageステージ"))
	{
		//	周波数データのデバッグ描画
		if (ImGui::TreeNode("Frequency Data"))
		{
			frequency_->DrawDebug();
			ImGui::DragInt("FrequencyIndex", &frequencyIndex_, 1.0f, 0);
			ImGui::DragFloat("CurrentFrequency", &currentFrequencyValue_, 1.0f, 0.0f);
			ImGui::DragFloat("FrequencyMin", &frequencyMinValue_, 1.0f, 0.0f);
			ImGui::DragFloat("FrequencyMax", &frequencyMaxValue_, 1.0f, 0.0f);
			ImGui::DragFloat("EmissiveIntencity", &emissiveConstant_.emissiveIntensity_, 0.1f, 0.0f, FLT_MAX);
			ImGui::DragFloat("EmissiveFactor", &emissiveFactor_, 1.0f, 0.0f);
			ImGui::DragFloat("EmissiveIntencityMin", &emissiveIntencityMin_, 1.0f, 0.0f);
			ImGui::DragFloat("EmissiveIntencityMax", &emissiveIntencityMax_, 1.0f, 0.0f);
		}

		gltfStaticModelResource_->DrawDebug();
		GetTransform()->DrawDebug();
		ImGui::TreePop();
	}
}
