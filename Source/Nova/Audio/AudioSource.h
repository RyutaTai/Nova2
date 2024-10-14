#pragma once

#include <memory>
#define XAUDIO2_HELPER_FUNCTIONS
#include <xaudio2.h>
#include <x3daudio.h>
#include "WaveRead.h"

// �I�[�f�B�I�\�[�X
class AudioSource
{
public:
	AudioSource(IXAudio2* xaudio, std::shared_ptr<WaveReader>& resource);
	~AudioSource();

public:
	
	// �X�V����
	void Update(FLOAT32 elapsedtime);

	// �Đ�
	void Play(BOOL loop);

	// �ĊJ
	void Restart();

	// ��~
	void Stop();

	// �ꎞ��~
	void Pause();

	// �t�B���^�[
	void Filter(XAUDIO2_FILTER_TYPE type, FLOAT32 cutoff = 7350.0f, FLOAT32 overq = 1.0f);

	// �^�C�}�[���Z
	void AddPlayTimer(FLOAT32 time) { play_timer += time; }
	void AddTotalPlayTimer(const FLOAT32& time) { totalPlayTimer_ += time; }

	virtual void DrawDebug();

public: // getter setter

	IXAudio2SourceVoice* GetSourceVoice() { return source_voice; }

	UINT32 GetPlayLength() const { return length; }
	FLOAT32 GetPlayLengthFLOAT() const { return length_float; }

	FLOAT32 GetPlayTimer() const { return play_timer; }
	void ResetPlayTimer() { play_timer = 0.0f; }

	FLOAT32 GetTotalPlayTimer()const { return totalPlayTimer_; }

	XAUDIO2_VOICE_SENDS* GetSfxsendlist() { return &SFXSendList; }

	FLOAT32 GetVolume() const { return last_volume; }

	XAUDIO2_VOICE_STATE GetState() const { return state; }

	float GetAudioBytes() const { return buffer.AudioBytes; }	//	�o�b�t�@�[�̃T�C�Y�擾

	size_t GetCurrentSample()const;		//	���݂̍Đ��ʒu���T���v���P�ʂŎ擾
	
	void SetVolume(FLOAT32 volume, BOOL use_db);

	virtual void SetPitch(FLOAT32 pitch);

	virtual void SetPan(FLOAT32 pan);

protected:

	// ��{�̃T���v�����O���[�g
	static constexpr FLOAT32 DEFAULT_SAMPLERATE = 44100.0f;

	// �\�[�X
	IXAudio2SourceVoice* source_voice = nullptr;

	// WAVE�t�H�[�}�b�g���
	WAVEFORMATEX wfe;

	// ���݂̍Đ�����(���f�[�^�̂ǂ��܂ōĐ�������)
	FLOAT32 play_timer = 0.0f;

	//	���܂ł̍Đ�����(���[�v���Ă����Z�b�g���Ȃ�)
	FLOAT32 totalPlayTimer_ = 0.0f;

	// �����̒���
	UINT32 length{};
	FLOAT32 length_float{};

	// �o�b�t�@�[
	XAUDIO2_BUFFER buffer = { 0 };

	// 
	INT32 max_output_matrix{ 8 };

	// �O�t���[�����_�ł̃{�����[�� : SetVolume���g���O�ɂ��̒l�Ɣ�ׂ�
	FLOAT32 last_volume{};

	// �t�B���^�[�̎��
	XAUDIO2_FILTER_PARAMETERS filter_parameters;

	XAUDIO2_SEND_DESCRIPTOR SFXSend;
	XAUDIO2_VOICE_SENDS SFXSendList{};
	XAUDIO2_VOICE_STATE state;

};