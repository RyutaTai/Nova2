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
		this->emitter_ = new SoundEmitter(*emitter); // �l�̂ݑ�� 	this->emitter = emitter;
		dspSetting_.srcChannelCount_ = resource->GetWaveForMatex().nChannels;
		dspSetting_.dstChannelCount_ = 2;
		dspSetting_.outputMatrix_ = new FLOAT32[dspSetting_.srcChannelCount_ * dspSetting_.dstChannelCount_];
	}
}

AudioSource3D::~AudioSource3D()
{
	if (emitter_ != nullptr) delete emitter_;
	if (dspSetting_.outputMatrix_ != nullptr) delete dspSetting_.outputMatrix_;
}

void AudioSource3D::Update(FLOAT32 elapsedtime)
{
	sourceVoice_->GetState(&state_, XAUDIO2_VOICE_NOSAMPLESPLAYED);

	//	�Đ����ԍX�V
	AddPlayTimer(elapsedtime);
	AddTotalPlayTimer(elapsedtime);

	//	�����f�[�^�̒����𒴂�����Đ����ԃ��Z�b�g
	float length = GetPlayLengthFloat();
	if (GetPlayTimer() >= length)
	{
		ResetPlayTimer();	
	}

	if (emitter_ != nullptr)
	{
		SetVolume(emitter_->volume_, false);
		
		SetPan();

		Filter(LowPassOnePoleFilter);
	}
	else // none emitter
	{
		SetVolume(emitter_->volume_, false);
	}
	DrawDebug();

	//	�s�b�`�ύX�e�X�g
#if 0	
	SetPitch(pitch_);							//	�s�b�`�ύX(�Đ����x���ς���Ă��܂�)�e�[�v�G�t�F�N�g�I�Ȃ̂ł���?
	//source_voice->SetFrequencyRatio(pitch_);	//	pitch�Ƀh�b�v���[���ʂ��|���Ă��Ȃ�

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
	
	XAUDIO2_VOICE_DETAILS voiceDetails;
	sourceVoice_->GetVoiceDetails(&voiceDetails);

	XAUDIO2_VOICE_DETAILS masterDetails;
	Audio::Instance().GetMasteringVoice()->GetVoiceDetails(&masterDetails);

	//	�}�X�^�����O�{�C�X���(�f�o�b�O�p)
	int masterInputChannel = masterDetails.InputChannels;
	int masterSampleRate = masterDetails.InputSampleRate;

#if 0
	float   volumes[] = { 1.0f, 0.0f,1.0f,0.0f };
	float pan = -90.0f;	//	�^��
	float rad = ((-90.0f + 90.0f) / 2.0f) * (M_PI / 180.0f);	//  ���W�A���ɕϊ�
	volumes[0] = cosf(rad);										//  ���{�����[��
	volumes[2] = cosf(rad);										//  ���{�����[��
	volumes[1] = sinf(rad);										//  �E�{�����[��
	volumes[3] = sinf(rad);										//  �E�{�����[��


	hr = source_voice->SetOutputMatrix(Audio::Instance().GetMasteringVoice(), voice_details.InputChannels, master_details.InputChannels, volumes);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
#else
	// ���X�i�[�Ɖ����̈ʒu�֌W����o�͐�z���K�p
	float   volumes[] = { 1.0f, 0.0f,1.0f,0.0f };
	for (int i = 0; i < 4; i++)
	{
		volumes[i] = dspSetting_.outputMatrix_[i];
	}
	sourceVoice_->SetOutputMatrix(Audio::Instance().GetMasteringVoice(), voiceDetails.InputChannels, masterDetails.InputChannels, dspSetting_.outputMatrix_);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

#endif

}

void AudioSource3D::Filter(XAUDIO2_FILTER_TYPE type, FLOAT32 overq)
{
	filterParameters_.Type = type; //�g���t�B���^�[�̎��

	filterParameters_.Frequency										//�J�b�g������g���̊(0Hz(0.0f) ~ 7350Hz(1.0f))
		= 2.0f * sinf(X3DAUDIO_PI / 6.0f * (1.0f - dspSetting_.filterParam_)); // ���X�i�[�Ɖ����̈ʒu�֌W����Ƃ����t�B���^�[�W����K�p

	filterParameters_.OneOverQ = overq; //���ۂɂǂ̂��炢�̉��ʂ��J�b�g����Ă��邩���w�肷��

	sourceVoice_->SetFilterParameters(&filterParameters_);
}

void AudioSource3D::DrawDebug()
{
#ifdef _DEBUG
	ImGui::Begin("3DEmitter");
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	ImGui::DragFloat3("EmitterVelocity", &emitter_->velocity_.x, -5.0f, 5.0f);
	ImGui::DragFloat3("EmitterPosition", &emitter_->position_.x);
	ImGui::DragFloat("EmitterMinDistance", &emitter_->minDistance_);
	ImGui::DragFloat("EmitterMaxDistance", &emitter_->maxDistance_);
	ImGui::SliderFloat("Volume", &emitter_->volume_, 0.0f, 1.0f);

	//	�f�o�b�O
	ImGui::DragFloat("Pitch", &pitch_, 0.01f, XAUDIO2_MIN_FREQ_RATIO, XAUDIO2_MAX_FREQ_RATIO);	//	�s�b�`�ύX�e�X�g

	//	�Đ����ԕ\��
	ImGui::DragFloat("TotalPlayTimer", &totalPlayTimer_);
	ImGui::DragFloat("PlayTimer", &playTimer_);

	ImGui::End();
	Graphics::Instance().GetDebugRenderer()->DrawSphere(emitter_->position_, emitter_->minDistance_, { 0.0f, 0.0f, 1.0f, 1.0f });
	Graphics::Instance().GetDebugRenderer()->DrawSphere(emitter_->position_, emitter_->maxDistance_, { 1.0f, 0.0f, 0.0f, 1.0f });

	ImGui::Begin("DSPSetting");
	ImGui::DragFloat("angle", &dspSetting_.radianListenerToEmitter_);
	ImGui::DragFloat("DopplerFactor", &dspSetting_.dopplerScale_);
	ImGui::DragFloat("Distance", &dspSetting_.distanceListnerToEmitter_);
	ImGui::DragFloat("FilterParam", &dspSetting_.filterParam_);
	ImGui::End();
#endif
}
