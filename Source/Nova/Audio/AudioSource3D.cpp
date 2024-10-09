#include "AudioSource3D.h"

#include <xaudio2.h>
#include <x3daudio.h>
#ifdef USE_IMGUI
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"
#include "../../imgui/imgui_impl_dx11.h"
#include "../../imgui/imgui_impl_win32.h"
#endif

#include "../Others/Misc.h"
#include "Audio.h"
#include "../Graphics/Graphics.h"

#include "PitchShifter.h"
#include "../../Game/Player.h"

AudioSource3D::AudioSource3D(IXAudio2* xaudio, std::shared_ptr<WaveReader>& resource, SoundEmitter* emitter) : AudioSource(xaudio, resource)
{
	if (emitter != nullptr)
	{
		this->emitter = new SoundEmitter(*emitter); // 値のみ代入 	this->emitter = emitter;
		dsp_setting.src_channel_count = resource->GetWaveForMatex().nChannels;
		dsp_setting.dst_channel_count = 2;
		dsp_setting.output_matrix = new FLOAT32[dsp_setting.src_channel_count * dsp_setting.dst_channel_count];
	}
}

AudioSource3D::~AudioSource3D()
{
	if (emitter != nullptr) delete emitter;
	if (dsp_setting.output_matrix != nullptr) delete dsp_setting.output_matrix;
}

void AudioSource3D::Update(FLOAT32 elapsedtime)
{
	source_voice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);

	//	再生時間更新
	AddPlayTimer(elapsedtime);
	AddTotalPlayTimer(elapsedtime);

	//	音源データの長さを超えたら再生時間リセット
	float length = GetPlayLengthFLOAT();
	if (GetPlayTimer() >= length)
	{
		ResetPlayTimer();	
	}

	if (emitter != nullptr)
	{
		SetVolume(emitter->volume_, false);
		
		SetPan();

		Filter(LowPassOnePoleFilter);
	}
	else // none emitter
	{
		SetVolume(emitter->volume_, false);
	}
	DebugGUI();

	//	ピッチ変更テスト
#if 0	
	SetPitch(pitch_);							//	ピッチ変更(再生速度も変わってしまう)テープエフェクト的なのできる?
	//source_voice->SetFrequencyRatio(pitch_);	//	pitchにドップラー効果を掛けていない

	PitchShifter* pitchShifter = new PitchShifter();
	std::vector<float> input = {};
	std::vector<float> output = {};
	//pitchShifter->ApplyPitchShift(pitch_, input, output);

	BYTE* data = Audio::Instance().GetResource()->GetAudioData();
#endif
}

void AudioSource3D::SetPan()
{
	HRESULT hr = S_OK;
	
	XAUDIO2_VOICE_DETAILS voice_details;
	source_voice->GetVoiceDetails(&voice_details);

	XAUDIO2_VOICE_DETAILS master_details;
	Audio::Instance().GetMasteringVoice()->GetVoiceDetails(&master_details);

	//	マスタリングボイス情報(デバッグ用)
	int masterInputChannel = master_details.InputChannels;
	int masterSampleRate = master_details.InputSampleRate;

#if 0
	float   volumes[] = { 1.0f, 0.0f,1.0f,0.0f };
	float pan = -90.0f;	//	真左
	float rad = ((-90.0f + 90.0f) / 2.0f) * (M_PI / 180.0f);	//  ラジアンに変換
	volumes[0] = cosf(rad);										//  左ボリューム
	volumes[2] = cosf(rad);										//  左ボリューム
	volumes[1] = sinf(rad);										//  右ボリューム
	volumes[3] = sinf(rad);										//  右ボリューム


	hr = source_voice->SetOutputMatrix(Audio::Instance().GetMasteringVoice(), voice_details.InputChannels, master_details.InputChannels, volumes);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
#else
	// リスナーと音源の位置関係から出力先配列を適用
	float   volumes[] = { 1.0f, 0.0f,1.0f,0.0f };
	for (int i = 0; i < 4; i++)
	{
		volumes[i] = dsp_setting.output_matrix[i];
	}
	source_voice->SetOutputMatrix(Audio::Instance().GetMasteringVoice(), voice_details.InputChannels, master_details.InputChannels, dsp_setting.output_matrix);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

#endif

}

void AudioSource3D::Filter(XAUDIO2_FILTER_TYPE type, FLOAT32 overq)
{
	filter_parameters.Type = type; //使うフィルターの種類

	filter_parameters.Frequency										//カットする周波数の基準(0Hz(0.0f) ~ 7350Hz(1.0f))
		= 2.0f * sinf(X3DAUDIO_PI / 6.0f * (1.0f - dsp_setting.filter_param)); // リスナーと音源の位置関係からとったフィルター係数を適用

	filter_parameters.OneOverQ = overq; //実際にどのくらいの音量がカットされているかを指定する

	source_voice->SetFilterParameters(&filter_parameters);
}

void AudioSource3D::DebugGUI()
{
#ifdef _DEBUG
	ImGui::Begin("3DEmitter");
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	ImGui::DragFloat3("EmitterVelocity", &emitter->velocity.x, -5.0f, 5.0f);
	ImGui::DragFloat3("EmitterPosition", &emitter->position.x);
	ImGui::DragFloat("EmitterMinDistance", &emitter->min_distance);
	ImGui::DragFloat("EmitterMaxDistance", &emitter->max_distance);
	ImGui::SliderFloat("Volume", &emitter->volume_, 0.0f, 1.0f);

	//	デバッグ
	ImGui::DragFloat("Pitch", &pitch_, 0.01f, XAUDIO2_MIN_FREQ_RATIO, XAUDIO2_MAX_FREQ_RATIO);	//	ピッチ変更テスト

	//	再生時間表示
	ImGui::DragFloat("TotalPlayTimer", &totalPlayTimer_);
	ImGui::DragFloat("PlayTimer", &play_timer);

	ImGui::End();
	Graphics::Instance().GetDebugRenderer()->DrawSphere(emitter->position, emitter->min_distance, { 0.0f, 0.0f, 1.0f, 1.0f });
	Graphics::Instance().GetDebugRenderer()->DrawSphere(emitter->position, emitter->max_distance, { 1.0f, 0.0f, 0.0f, 1.0f });

	ImGui::Begin("DSPSetting");
	ImGui::DragFloat("angle", &dsp_setting.radian_listener_to_emitter);
	ImGui::DragFloat("DopplerFactor", &dsp_setting.doppler_scale);
	ImGui::DragFloat("Distance", &dsp_setting.distance_listner_to_emitter);
	ImGui::DragFloat("FilterParam", &dsp_setting.filter_param);
	ImGui::End();
#endif
}
