#pragma once

#include <memory>

#include "../Nova/Resources/GltfModelStaticBatching.h"
#include "../Nova/Collision/CollisionMesh.h"
#include "../Nova/Resources/Midi.h"
#include "../Nova/Audio/Frequency.h"
#include "../Nova/Graphics/FrameBuffer.h"
#include "../Nova/Graphics/FullScreenQuad.h"

#define MAGIC_CIRCLE 0

class Stage
{
public:
	Stage();
	~Stage() {}

	static Stage& Instance();

	void ShadowRender(const float& scale = 1.0f);
	void Update(const float& elapsedTime);
	void Render();
	void DrawDebug();

	void UpdateFFTConstantBuffer();					//	FFT定数バッファ更新処理
	void UpdateEmissive(const float& elapsedTime);	//	エミッシブ更新処理
	void UpdateFrequencyMin();
	void UpdateFrequencyMax();
	float CalculateAutocorrelation(const float data[], const int& lag);

	bool Collision(_In_ const DirectX::XMFLOAT3& rayPosition, _In_ const DirectX::XMFLOAT3& rayDirection, _In_ const DirectX::XMFLOAT4X4& stageTransform, _Out_ DirectX::XMFLOAT3& intersectionPosition, _Out_ DirectX::XMFLOAT3& intersectionNormal,
		_Out_ std::string& intersectionMesh, _Out_ std::string& intersectionMaterial, _In_ float rayLengthLimit = 1.0e+7f, _In_ bool skipIf = false/*Once the first intersection is found, the process is interrupted.*/) const;

	Transform* GetTransform() { return gltfStaticModelResource_->GetTransform(); }
	Frequency* GetFrequency() { return frequency_.get(); }	//	音の周波数データ取得

private:
	enum class CollisionModel
	{
		Ground,
		Building,
		Max,
	};

	struct EmissiveConstant
	{
		float emissiveIntensity_;
		float dummy_[3];
	};
	EmissiveConstant emissiveConstant_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> emissiveConstantBuffer_;

	struct FFTConstant
	{
		float fftData_[Frequency::BlockCount];	//	FFTのデータを分割数分GPUに渡す
	};
	FFTConstant fftConstant_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> fftConstantBuffer_;

private:
	static Stage* instance_;

	std::shared_ptr<GltfModelStaticBatching>	gltfStaticModelResource_;		//	Gltfモデル
	std::unique_ptr<CollisionMesh>				collisionMesh_;

	//	FFT
	std::unique_ptr<FullScreenQuad>				bitBlockTransfer_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	spectrumPS_;
	std::unique_ptr<FrameBuffer>				spectrumFramebuffer_;
#if MAGIC_CIRCLE
	const int SPECTRUM_WIDTH = 512;
	const int SPECTRUM_HEIGHT = 512;
#else
	const int SPECTRUM_WIDTH = 2048;
	const int SPECTRUM_HEIGHT = 2048;
#endif

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> fftSRV_;	// projectionMapping

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> projectionMappingTexture_;	//	画像をロードして使う
	float fftDivisionValue_ = 10000.0f;	//	GPUに渡すFFTデータを割る値

	bool				useFrequency_			= true;
	static const int	FrequencyDataMax		= 120;
	float				frequencyData_[FrequencyDataMax];
	int					frequencyIndex_			= 265;
	//int					frequencyIndex_			= 509;
	float				currentFrequencyValue_	= 0.0f;
	float				frequencyMinValue_		= 0.0f;
	float				frequencyMaxValue_		= 0.0f;
	float				emissiveFactor_			= 15.0f;
	float				emissiveIntencityMin_	= 0.1f;
	float				emissiveIntencityMax_	= 15.0f;
	std::unique_ptr<Midi> midi_ = nullptr;				//	emissiveタイミング判定用midi

	std::unique_ptr<Frequency> frequency_ = nullptr;	//	音の周波数データ(emissiveIntencityの計算に使う)


	float threshold_ = 2100.0f;
	float defaultEmissiveIntensity_ = 10.0f;
};