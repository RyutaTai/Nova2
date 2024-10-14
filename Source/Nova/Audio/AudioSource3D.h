#pragma once
#include "AudioSource.h"
#include "Audio3DSystem.h"


class AudioSource3D : public AudioSource
{
public:
	AudioSource3D(IXAudio2* xaudio, std::shared_ptr<WaveReader>& resource, SoundEmitter* emitter);
	~AudioSource3D();

public:
	void Update(FLOAT32 elapsedtime);
		
	void DrawDebug();

public:
	void SetDSPSetting(SoundListener& listner)
	{
		DSP(dsp_setting, listner, *emitter);
	}

	void SetEmitter(SoundEmitter emitter) { this->emitter = &emitter; }

	void SetPosition(DirectX::XMFLOAT3 position) { emitter->position = position; }

	void SetVelocity(DirectX::XMFLOAT3 velocity) { emitter->velocity = velocity; }

	void SetCurveDistanceScaler(FLOAT32 scaler) { emitter->max_distance = scaler; }

	// ドップラー効果を適用
	void SetPitch(FLOAT32 pitch) override { source_voice->SetFrequencyRatio(dsp_setting.doppler_scale * pitch); }

	void SetPan();

	void SetReflectionRate(FLOAT32 reflection_rate) { this->reflection_rate = reflection_rate; }

	void SetAbsortRate(FLOAT32 absorption_rate) { this->absorption_rate = absorption_rate; }

	/*void SetVolumeCurve(X3DAUDIO_DISTANCE_CURVE_POINT vol_curvepoint[], UINT32 array_size)
	{
		if (array_size > 0)
		{
			this->vol_curvepoints.resize(array_size);
			for (uint32_t i = 0; i < array_size; ++i)
			{
				this->vol_curvepoints.at(i) = vol_curvepoint[i];
			}
			vol_curve = { &this->vol_curvepoints.at(0), array_size };
			emitter->pVolumeCurve = &vol_curve;
		}
	}

	void SetLPFDirectCurve(X3DAUDIO_DISTANCE_CURVE_POINT lpfdirect_curvepoint[], UINT32 array_size)
	{
		if (array_size)
		{
			this->lpfdirect_curvepoints.resize(array_size);
			for (uint32_t i = 0; i < array_size; ++i)
			{
				this->lpfdirect_curvepoints.at(i) = lpfdirect_curvepoint[i];
			}
			lpfdirect_curve = { &this->lpfdirect_curvepoints.at(0), array_size };
			emitter->pLPFDirectCurve = &lpfdirect_curve;
		}
	}*/

	void Filter(XAUDIO2_FILTER_TYPE type, FLOAT32 overq = 1.0f);

	SoundDSPSetting GetDSPSetting() const { return dsp_setting; }

	SoundEmitter* GetEmitter() { return emitter; }

private:

	SoundEmitter* emitter{};

	SoundDSPSetting dsp_setting{};

	FLOAT32 absorption_rate{};		//	吸収率

	FLOAT32 reflection_rate{};		//	反射率

	std::vector<X3DAUDIO_DISTANCE_CURVE_POINT> vol_curvepoints;
	X3DAUDIO_DISTANCE_CURVE vol_curve = { nullptr, 0 };

	std::vector<X3DAUDIO_DISTANCE_CURVE_POINT> lpfdirect_curvepoints;
	X3DAUDIO_DISTANCE_CURVE lpfdirect_curve = { nullptr, 0 };

	//	デバッグ用
	float pitch_ = 1.0f;

};