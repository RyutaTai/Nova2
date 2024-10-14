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
	void Update(FLOAT32 elapsedTime);

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
	void AddPlayTimer(FLOAT32 elapsedTime) { playTimer_ += elapsedTime; }
	void AddTotalPlayTimer(const FLOAT32& elapsedTime) { totalPlayTimer_ += elapsedTime; }

	virtual void DrawDebug();

public: // getter setter

	IXAudio2SourceVoice* GetSourceVoice() { return sourceVoice_; }

	UINT32 GetPlayLength() const { return length_; }
	FLOAT32 GetPlayLengthFLOAT() const { return lengthFloat_; }

	FLOAT32 GetPlayTimer() const { return playTimer_; }
	void ResetPlayTimer() { playTimer_ = 0.0f; }

	FLOAT32 GetTotalPlayTimer()const { return totalPlayTimer_; }

	XAUDIO2_VOICE_SENDS* GetSfxsendlist() { return &SFXSendList_; }

	FLOAT32 GetVolume() const { return lastVolume_; }

	XAUDIO2_VOICE_STATE GetState() const { return state_; }

	float GetAudioBytes() const { return buffer_.AudioBytes; }	//	�o�b�t�@�[�̃T�C�Y�擾

	size_t GetCurrentSample()const;		//	���݂̍Đ��ʒu���T���v���P�ʂŎ擾
	
	bool IsPlay();

	void SetVolume(FLOAT32 volume, BOOL useDb);

	virtual void SetPitch(FLOAT32 pitch);

	virtual void SetPan(FLOAT32 pan);

protected:

	// ��{�̃T���v�����O���[�g
	static constexpr FLOAT32 DEFAULT_SAMPLERATE = 44100.0f;

	// �\�[�X
	IXAudio2SourceVoice* sourceVoice_ = nullptr;

	// WAVE�t�H�[�}�b�g���
	WAVEFORMATEX wfe_;

	// ���݂̍Đ�����(���f�[�^�̂ǂ��܂ōĐ�������)
	FLOAT32 playTimer_ = 0.0f;

	//	���܂ł̍Đ�����(���[�v���Ă����Z�b�g���Ȃ�)
	FLOAT32 totalPlayTimer_ = 0.0f;

	// �����̒���
	UINT32 length_ = {};
	FLOAT32 lengthFloat_ = {};

	// �o�b�t�@�[
	XAUDIO2_BUFFER buffer_ = { 0 };

	// 
	INT32 maxOutputMatrix_{ 8 };

	// �O�t���[�����_�ł̃{�����[�� : SetVolume���g���O�ɂ��̒l�Ɣ�ׂ�
	FLOAT32 lastVolume_{};

	// �t�B���^�[�̎��
	XAUDIO2_FILTER_PARAMETERS filterParameters_;

	XAUDIO2_SEND_DESCRIPTOR SFXSend_;
	XAUDIO2_VOICE_SENDS SFXSendList_ = {};
	XAUDIO2_VOICE_STATE state_;

	bool isPlaying_ = false;	//	�Đ������ǂ����̃t���O

};