#pragma once

#include <memory>
#include <string>

#define XAUDIO2_HELPER_FUNCTIONS
#include <xaudio2.h>
#include <x3daudio.h>

#include "WaveRead.h"

//	オーディオソース
class AudioSource
{
public:
	enum class AudioType
	{
		BGMNormal,
		SENormal,
		BGM3D,
		SE3D,
		Max
	};

public:
	AudioSource(IXAudio2* xaudio, WaveReader* resource, const AudioType& audioType = {}, const std::string& sceneName = {});
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
	void AddPlayTimer(const FLOAT32& elapsedTime) { timer_ += elapsedTime; }
	void AddTotalPlayTimer(const FLOAT32& elapsedTime) { totalPlayTimer_ += elapsedTime; }

	virtual void DrawDebug();

public: // getter setter

	void ResetPlayTimer() { timer_ = 0.0f; }

	IXAudio2SourceVoice*	GetSourceVoice()			{ return sourceVoice_; }
	UINT32					GetPlayLength()		const	{ return length_; }
	FLOAT32					GetPlayLengthFloat()const	{ return lengthFloat_; }
	FLOAT32					GetPlayTimer()		const	{ return timer_; }
	FLOAT32					GetTotalPlayTimer()	const	{ return totalPlayTimer_; }
	XAUDIO2_VOICE_SENDS*	GetSfxSendList()			{ return &SFXSendList_; }
	FLOAT32					GetVolume()			const	{ return lastVolume_; }
	const WAVEFORMATEX&		GetWaveFormat()		const	{ return wfx_; }				//	WAVEフォーマット取得
	XAUDIO2_VOICE_STATE		GetState()			const	{ return state_; }
	const BYTE*				GetAudioData()		const	{ return buffer_.pAudioData; }
	size_t					GetAudioBytes()		const	{ return buffer_.AudioBytes; }	//	バッファーのサイズ取得
	size_t					GetCurrentSample()	const;									//	現在の再生位置をサンプル単位で取得
	std::string				GetName()			const	{ return name_; }

	bool					IsPlay();

	void			SetVolume(FLOAT32 volume, BOOL useDb);
	virtual void	SetPitch(FLOAT32 pitch);
	virtual void	SetPan(FLOAT32 pan);

	void			SetAudioType(const AudioType& type) { audioType_ = type; }
	AudioType		GetMyAudioType() { return audioType_; }
	bool			IsBGM();	//	BGMかどうか
	bool			IsSE();		//	SEかどうか

	void			SetSceneName(const std::string& sceneName) { sceneName_ = sceneName; }
	std::string		GetSceneName() { return sceneName_; }

protected:

	//	基本のサンプリングレート
	static constexpr FLOAT32 DEFAULT_SAMPLERATE = 44100.0f;

	//	ソース
	IXAudio2SourceVoice* sourceVoice_ = nullptr;

	//	WAVEフォーマット情報
	WAVEFORMATEX wfx_;

	//	現在の再生時間(音データのどこまで再生したか)
	FLOAT32 timer_ = 0.0f;

	//	今までの再生時間(ループしてもリセットしない)
	FLOAT32 totalPlayTimer_ = 0.0f;

	//	音源の長さ
	UINT32 length_ = {};
	FLOAT32 lengthFloat_ = {};

	//	バッファー
	XAUDIO2_BUFFER buffer_ = { 0 };

	//	音源の名前
	std::string name_ = {};

	static constexpr int OutputMatrixMax_ = 8;	//	出力マトリックス最大数

	//	前フレーム時点でのボリューム : SetVolumeを使う前にこの値と比べる
	FLOAT32 lastVolume_ = {};

	//	フィルターの種類
	XAUDIO2_FILTER_PARAMETERS filterParameters_;

	XAUDIO2_SEND_DESCRIPTOR SFXSend_;
	XAUDIO2_VOICE_SENDS SFXSendList_ = {};
	XAUDIO2_VOICE_STATE state_;

	bool isPlaying_ = false;	//	再生中かどうかのフラグ

	AudioType	audioType_ = {};	//	オーディオタイプ
	std::string	sceneName_ = {};	//	使用シーンを設定(Title,Gameなど)

};