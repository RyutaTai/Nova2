#include "Audio.h"

#include <xaudio2fx.h>
#include "../Others/Misc.h"

// �R���X�g���N�^
Audio::Audio()
{
	HRESULT hr;

	// COM�̏�����
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	UINT32 createFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createFlags |= XAUDIO2_DEBUG_ENGINE;
#endif

	// XAudio������
	hr = XAudio2Create(&xaudio_, createFlags);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// create masteringvoice
#if 0 //	�������o
	hr = xaudio->CreateMasteringVoice(&masteringVoice, XAUDIO2_DEFAULT_CHANNELS, 44100/*�T���v�����O���[�g*/, 0U, NULL, 0, AudioCategory_GameEffects);
#else
	hr = xaudio_->CreateMasteringVoice(&masteringVoice_, 2, 44100/*�T���v�����O���[�g*/, 0U, NULL, 0, AudioCategory_GameEffects);
#endif
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	masteringVoice_->GetChannelMask(&channelMask_);

	/*FLOAT32 PeakLevels[2]{};
	FLOAT32 RMSLevels[2]{};

	XAUDIO2FX_VOLUMEMETER_LEVELS levels;

	levels.pPeakLevels = PeakLevels;
	levels.pRMSLevels = RMSLevels;
	levels.ChannelCount = 2;

	masteringVoice->GetEffectParameters(0, &levels, sizeof(levels));*/


	/*IUnknown* pXAPO_volumemeter;
	hr = XAudio2CreateVolumeMeter(&pXAPO_volumemeter);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	XAUDIO2_EFFECT_DESCRIPTOR descripter;
	descripter.InitialState = true;
	descripter.OutputChannels = 2;
	descripter.pEffect = pXAPO_volumemeter;

	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = 1;
	chain.pEffectDescriptors = &descripter;

	masteringVoice->SetEffectChain(&chain);
	pXAPO_volumemeter->Release();*/



}

// �f�X�g���N�^
Audio::~Audio()
{
	// �}�X�^�����O�{�C�X�j��
	if (masteringVoice_ != nullptr)
	{
		masteringVoice_->DestroyVoice();
		masteringVoice_ = nullptr;
	}

	// XAudio�I����
	if (xaudio_ != nullptr)
	{
		xaudio_->Release();
		xaudio_ = nullptr;
	}

	// COM�I����
	CoUninitialize();
}

// �I�[�f�B�I�\�[�X�ǂݍ���
AudioSource* Audio::LoadAudioSource(const char* filename)
{
	resource_ = std::make_shared<WaveReader>(filename);
	return new AudioSource(xaudio_, resource_);
}
AudioSource3D* Audio::LoadAudioSource3D(const char* filename, SoundEmitter* emitter)
{
	resource_ = std::make_shared<WaveReader>(filename);
	return new AudioSource3D(xaudio_, resource_, emitter);
}
