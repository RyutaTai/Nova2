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
	void Update(FLOAT32 elapsedtime);

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

	float GetAudioBytes() const { return buffer.AudioBytes; }	//	バッファーのサイズ取得

	size_t GetCurrentSample()const;		//	現在の再生位置をサンプル単位で取得
	
	void SetVolume(FLOAT32 volume, BOOL use_db);

	virtual void SetPitch(FLOAT32 pitch);

	virtual void SetPan(FLOAT32 pan);

protected:

	// 基本のサンプリングレート
	static constexpr FLOAT32 DEFAULT_SAMPLERATE = 44100.0f;

	// ソース
	IXAudio2SourceVoice* source_voice = nullptr;

	// WAVEフォーマット情報
	WAVEFORMATEX wfe;

	// 現在の再生時間(音データのどこまで再生したか)
	FLOAT32 play_timer = 0.0f;

	//	今までの再生時間(ループしてもリセットしない)
	FLOAT32 totalPlayTimer_ = 0.0f;

	// 音源の長さ
	UINT32 length{};
	FLOAT32 length_float{};

	// バッファー
	XAUDIO2_BUFFER buffer = { 0 };

	// 
	INT32 max_output_matrix{ 8 };

	// 前フレーム時点でのボリューム : SetVolumeを使う前にこの値と比べる
	FLOAT32 last_volume{};

	// フィルターの種類
	XAUDIO2_FILTER_PARAMETERS filter_parameters;

	XAUDIO2_SEND_DESCRIPTOR SFXSend;
	XAUDIO2_VOICE_SENDS SFXSendList{};
	XAUDIO2_VOICE_STATE state;

};