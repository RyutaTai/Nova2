#include "AudioSource.h"
#include "../Others/Misc.h"
#include "Audio.h"
#include "../Graphics/Graphics.h"

// �R���X�g���N�^
AudioSource::AudioSource(IXAudio2* xaudio, std::shared_ptr<WaveReader>& resource)
{
	HRESULT hr;

	wfe = resource->GetWaveForMatex();

	// �\�[�X�{�C�X�𐶐�
	hr = xaudio->CreateSourceVoice(
		&source_voice,							//	�\�[�X�{�C�X�̃C���X�^���X��Ԃ�(�K�{)
		&wfe,									//	�\�[�X�{�C�X�֓n��WAVEFORMATRIX�̍\���̂�Ԃ�(�K�{)
		XAUDIO2_VOICE_USEFILTER,				//	�\�[�X�{�C�X�֎g�p������ʂ��w�肷��
		XAUDIO2_MAX_FREQ_RATIO					//	�ő勖�e�Đ����x���w�� �f�t�H���g��2.0f, �ő��XAUDIO2_MAX_FREQ_RATIO(1024.0f)�܂Őݒ�\
	);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// �T���v�����O���[�g�ɍ��킹�ăs�b�`��ύX(�덷���o�����Ɏg����A�}�W�ł���Ȃ�)
	// sourceVoice->SetFrequencyRatio(DEFAULT_SAMPLERATE / static_cast<FLOAT32>(resource->GetWaveFormat().nSamplesPerSec));
	length = resource->GetPlayLength();
	length_float = resource->GetPlayLengthFLOAT();
	// �\�[�X�{�C�X�Ƀf�[�^�𑗐M
	buffer.pAudioData = resource->GetAudioData();
	buffer.AudioBytes = resource->GetAudioBytes();
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	//buffer.PlayLength = resource->GetWaveFormat().nSamplesPerSec * resource->GetPlayLength();
	//buffer.PlayBegin = resource->GetWaveFormat().nSamplesPerSec * 30; // �Đ��J�n�ʒu�̎w�� �T���v���P��(�T���v�����O���[�g * �b��)�Ŏw��

	SFXSend = { 0, source_voice };
	SFXSendList = { 1, &SFXSend };

}

// �f�X�g���N�^
AudioSource::~AudioSource()
{
	if (source_voice != nullptr)
	{
		source_voice->DestroyVoice();
		source_voice = nullptr;
	}

	delete buffer.pAudioData;

}

void AudioSource::Update(FLOAT32 elapsedtime)
{
	source_voice->GetState(&state);
	
	//if (state.BuffersQueued == 1)
	{
		DebugGUI();
		AddPlayTimer(elapsedtime);
	}

	//INT32 CurrentDiskReadBuffer = 0;
	//INT32 CurrentPosition = 0;
	//while (CurrentPosition < resource->GetWaveFormat().cbSize)
	//{
	//	DWORD cbValid = min(STREAMING_BUFFER_SIZE, resource->GetWaveFormat().cbSize - CurrentPosition);
	//	DWORD dwRead;
	//	if (0 == ReadFile(hFile, buffers[CurrentDiskReadBuffer], STREAMING_BUFFER_SIZE, &dwRead, &overlapped))
	//		hr = HRESULT_FROM_WIN32(GetLastError());
	//	Overlapped.Offset += cbValid;

	//	//update the file position to where it will be once the read finishes
	//	CurrentPosition += cbValid;

	//	DWORD NumberBytesTransferred;
	//	::GetOverlappedResult(hFile, &Overlapped, &NumberBytesTransferred, TRUE);

	//	while (state.BuffersQueued >= MAX_BUFFER_COUNT - 1)
	//	{
	//		WaitForSingleObject(Context.hBufferEndEvent, INFINITE);
	//	}

	//	buffer.AudioBytes = cbValid;
	//	buffer.pAudioData = buffers[CurrentDiskReadBuffer];
	//	if (CurrentPosition >= resource->GetWaveFormat().cbSize)
	//	{
	//		buffer.Flags = XAUDIO2_END_OF_STREAM;
	//	}
	//	source_voice->SubmitSourceBuffer(&buffer);

	//	CurrentDiskReadBuffer++;
	//	CurrentDiskReadBuffer %= MAX_BUFFER_COUNT;
	//}

}

// �Đ�
void AudioSource::Play(BOOL loop)
{
	buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
	source_voice->SubmitSourceBuffer(&buffer);
	HRESULT hr = source_voice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	play_timer = 0.0f;
}

// �ĊJ
void AudioSource::Restart()
{
	source_voice->Start();
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//sourceVoice->SetVolume(volume);
}

// ��~
void AudioSource::Stop()
{
	source_voice->Stop();
	source_voice->FlushSourceBuffers();
	source_voice->SubmitSourceBuffer(&buffer);
	state.SamplesPlayed = 0;
}


// �ꎞ��~
void AudioSource::Pause()
{
	source_voice->Stop(XAUDIO2_PLAY_TAILS); // ������XAUDIO2_PLAY_TAILS��ݒ肷�邱�Ƃł���������(���o�[�u�̎c���Ȃ�)���c��
}

//	���݂̍Đ��ʒu���T���v���P�ʂŎ擾
size_t AudioSource::GetCurrentSample()const
{
	//���݂��T���v���P�ʂŌv�Z
	XAUDIO2_VOICE_STATE vs;
	source_voice->GetState(&vs);

	return (size_t(vs.SamplesPlayed) * size_t(wfe.nBlockAlign)) % buffer.AudioBytes;	//	�v����
}

void AudioSource::SetVolume(FLOAT32 volume, BOOL use_db)
{
	if (use_db)
	{
		if (volume <= -40.0f)
			volume = 0.0f;
		else
			volume = XAudio2DecibelsToAmplitudeRatio(volume); // dB��0.0f~1.0f�ɕϊ�
	}
	if (volume != last_volume)// SetVolume���d�������̂��ߕ��򂳂��Ă���
	{
		source_voice->SetVolume(volume);
		last_volume = volume;
	}


}

void AudioSource::SetPitch(FLOAT32 pitch)
{
	source_voice->SetFrequencyRatio(pitch);

}

void AudioSource::SetPan(FLOAT32 pan)
{
	FLOAT32 output_pan[8];
	for (int i = 0; i < max_output_matrix; i++) output_pan[i] = 0;

	XAUDIO2_VOICE_DETAILS voice_details;
	source_voice->GetVoiceDetails(&voice_details);

	XAUDIO2_VOICE_DETAILS master_details;
	Audio::Instance().GetMasteringVoice()->GetVoiceDetails(&master_details);

	FLOAT32 left{ 0.5f - pan * 0.5f };
	FLOAT32 right{ 0.5f + pan * 0.5f };

	switch (Audio::Instance().GetCannelmask())
	{
	case SPEAKER_MONO:
		output_pan[0] = 1.0;
		break;

	case SPEAKER_STEREO:
	case SPEAKER_2POINT1:
	case SPEAKER_SURROUND:
		output_pan[0] = left;
		output_pan[3] = right;
		break;

	case SPEAKER_QUAD:
		output_pan[0] = output_pan[2] = left;
		output_pan[1] = output_pan[3] = right;
		break;

	case SPEAKER_4POINT1:
		output_pan[0] = output_pan[3] = left;
		output_pan[1] = output_pan[4] = right;
		break;

	case SPEAKER_5POINT1:
	case SPEAKER_7POINT1:
	case SPEAKER_5POINT1_SURROUND:
		output_pan[0] = output_pan[4] = left;
		output_pan[1] = output_pan[5] = right;
		break;

	case SPEAKER_7POINT1_SURROUND:
		output_pan[0] = output_pan[4] = output_pan[6] = left;
		output_pan[1] = output_pan[5] = output_pan[7] = right;
		break;
	}

	source_voice->SetOutputMatrix(Audio::Instance().GetMasteringVoice(), voice_details.InputChannels, master_details.InputChannels, output_pan);

}

void AudioSource::Filter(XAUDIO2_FILTER_TYPE type, FLOAT32 cutoff, FLOAT32 overq)
{

	filter_parameters.Type = type; //�g���t�B���^�[�̎��
	filter_parameters.Frequency										//�J�b�g������g���̊(0Hz(0.0f) ~ 7350Hz(1.0f))
		= cutoff / wfe.nSamplesPerSec * 6.0f; //��:�J�b�g�I�t���g�� / �T���v�����O���[�g * 6.0f �� : 7350 / 44100 * 6.0f = 1.0f(���m�ɂ�1.000002f�����܂�����ł���)
																	//�P�Ƀt�B���^�[���g���ꍇ��XAudio2CutoffFrequencyToOnePoleCoefficient()�Ƃ����}�N�����g��(XAUDIO2_HELPER_FUNCTIONS���K�v)
				 
	filter_parameters.OneOverQ = overq; //���ۂɂǂ̂��炢�̉��ʂ��J�b�g����Ă��邩���w�肷��

	source_voice->SetFilterParameters(&filter_parameters);

}

void AudioSource::DebugGUI()
{
#ifdef USE_IMGUI
#endif
}
