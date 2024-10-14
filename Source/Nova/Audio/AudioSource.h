#pragma once

#include <memory>
#define XAUDIO2_HELPER_FUNCTIONS
#include <xaudio2.h>
#include <x3daudio.h>
#include "WaveRead.h"

// オーディオソース
class AudioSource
{
public:
	AudioSource(IXAudio2* xaudio, std::shared_ptr<WaveReader>& resource);
	~AudioSource();

public:
	
	// 更新処理
	void Update(FLOAT32 elapsedTime);

	// 再生
	void Play(BOOL loop);

	// 再開
	void Restart();

	// 停止
	void Stop();

	// 一時停止
	void Pause();

	// フィルター
	void Filter(XAUDIO2_FILTER_TYPE type, FLOAT32 cutoff = 7350.0f, FLOAT32 overq = 1.0f);

	// タイマー加算
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

	float GetAudioBytes() const { return buffer_.AudioBytes; }	//	バッファーのサイズ取得

	size_t GetCurrentSample()const;		//	現在の再生位置をサンプル単位で取得
	
	bool IsPlay();

	void SetVolume(FLOAT32 volume, BOOL useDb);

	virtual void SetPitch(FLOAT32 pitch);

	virtual void SetPan(FLOAT32 pan);

protected:

	// 基本のサンプリングレート
	static constexpr FLOAT32 DEFAULT_SAMPLERATE = 44100.0f;

	// ソース
	IXAudio2SourceVoice* sourceVoice_ = nullptr;

	// WAVEフォーマット情報
	WAVEFORMATEX wfe_;

	// 現在の再生時間(音データのどこまで再生したか)
	FLOAT32 playTimer_ = 0.0f;

	//	今までの再生時間(ループしてもリセットしない)
	FLOAT32 totalPlayTimer_ = 0.0f;

	// 音源の長さ
	UINT32 length_ = {};
	FLOAT32 lengthFloat_ = {};

	// バッファー
	XAUDIO2_BUFFER buffer_ = { 0 };

	// 
	INT32 maxOutputMatrix_{ 8 };

	// 前フレーム時点でのボリューム : SetVolumeを使う前にこの値と比べる
	FLOAT32 lastVolume_{};

	// フィルターの種類
	XAUDIO2_FILTER_PARAMETERS filterParameters_;

	XAUDIO2_SEND_DESCRIPTOR SFXSend_;
	XAUDIO2_VOICE_SENDS SFXSendList_ = {};
	XAUDIO2_VOICE_STATE state_;

	bool isPlaying_ = false;	//	再生中かどうかのフラグ

};