#include "AudioSource.h"
#include "../Others/Misc.h"
#include "Audio.h"
#include "../Graphics/Graphics.h"

// コンストラクタ
AudioSource::AudioSource(IXAudio2* xaudio, std::shared_ptr<WaveReader>& resource)
{
	HRESULT hr;

	wfe_ = resource->GetWaveForMatex();

	// ソースボイスを生成
	hr = xaudio->CreateSourceVoice(
		&sourceVoice_,							//	ソースボイスのインスタンスを返す(必須)
		&wfe_,									//	ソースボイスへ渡すWAVEFORMATRIXの構造体を返す(必須)
		XAUDIO2_VOICE_USEFILTER,				//	ソースボイスへ使用する効果を指定する
		XAUDIO2_MAX_FREQ_RATIO					//	最大許容再生速度を指定 デフォルトで2.0f, 最大でXAUDIO2_MAX_FREQ_RATIO(1024.0f)まで設定可能
	);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// サンプリングレートに合わせてピッチを変更(誤差が出た時に使える、マジでいらない)
	// sourceVoice->SetFrequencyRatio(DEFAULT_SAMPLERATE / static_cast<FLOAT32>(resource->GetWaveFormat().nSamplesPerSec));
	length_ = resource->GetPlayLength();
	lengthFloat_ = resource->GetPlayLengthFLOAT();
	// ソースボイスにデータを送信
	buffer_.pAudioData = resource->GetAudioData();
	buffer_.AudioBytes = resource->GetAudioBytes();
	buffer_.Flags = XAUDIO2_END_OF_STREAM;
	//buffer.PlayLength = resource->GetWaveFormat().nSamplesPerSec * resource->GetPlayLength();
	//buffer.PlayBegin = resource->GetWaveFormat().nSamplesPerSec * 30; // 再生開始位置の指定 サンプル単位(サンプリングレート * 秒数)で指定

	SFXSend_  = { 0, sourceVoice_ };
	SFXSendList_  = { 1, &SFXSend_ };

}

// デストラクタ
AudioSource::~AudioSource()
{
	if (sourceVoice_ != nullptr)
	{
		sourceVoice_->DestroyVoice();
		sourceVoice_ = nullptr;
	}

	delete buffer_.pAudioData;

}

void AudioSource::Update(FLOAT32 elapsedtime)
{
	sourceVoice_->GetState(&state_);
	
	//if (state.BuffersQueued == 1)
	{
		DrawDebug();
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

// 再生
void AudioSource::Play(BOOL loop)
{
	buffer_.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
	sourceVoice_->SubmitSourceBuffer(&buffer_);
	HRESULT hr = sourceVoice_->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	playTimer_ = 0.0f;
}

// 再開
void AudioSource::Restart()
{
	sourceVoice_->Start();
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//sourceVoice->SetVolume(volume);
}

// 停止
void AudioSource::Stop()
{
	sourceVoice_->Stop();
	sourceVoice_->FlushSourceBuffers();
	sourceVoice_->SubmitSourceBuffer(&buffer_);
	state_.SamplesPlayed = 0;
}


// 一時停止
void AudioSource::Pause()
{
	sourceVoice_->Stop(XAUDIO2_PLAY_TAILS); // 引数にXAUDIO2_PLAY_TAILSを設定することでかけた効果(リバーブの残響など)を残す
}

//	現在の再生位置をサンプル単位で取得
size_t AudioSource::GetCurrentSample()const
{
	//現在をサンプル単位で計算
	XAUDIO2_VOICE_STATE vs;
	sourceVoice_->GetState(&vs);

	return (size_t(vs.SamplesPlayed) * size_t(wfe_.nBlockAlign)) % buffer_.AudioBytes;	//	要調整
}

void AudioSource::SetVolume(FLOAT32 volume, BOOL use_db)
{
	if (use_db)
	{
		if (volume <= -40.0f)
			volume = 0.0f;
		else
			volume = XAudio2DecibelsToAmplitudeRatio(volume); // dBを0.0f~1.0fに変換
	}
	if (volume != lastVolume_)// SetVolumeが重い処理のため分岐させておく
	{
		sourceVoice_->SetVolume(volume);
		lastVolume_ = volume;
	}


}

void AudioSource::SetPitch(FLOAT32 pitch)
{
	sourceVoice_->SetFrequencyRatio(pitch);

}

void AudioSource::SetPan(FLOAT32 pan)
{
	FLOAT32 outputPan[8];
	for (int i = 0; i < maxOutputMatrix_; i++) outputPan[i] = 0;

	XAUDIO2_VOICE_DETAILS voiceDetails;
	sourceVoice_->GetVoiceDetails(&voiceDetails);

	XAUDIO2_VOICE_DETAILS masterDetails;
	Audio::Instance().GetMasteringVoice()->GetVoiceDetails(&masterDetails);

	FLOAT32 left{ 0.5f - pan * 0.5f };
	FLOAT32 right{ 0.5f + pan * 0.5f };

	switch (Audio::Instance().GetCannelmask())
	{
	case SPEAKER_MONO:
		outputPan[0] = 1.0;
		break;

	case SPEAKER_STEREO:
	case SPEAKER_2POINT1:
	case SPEAKER_SURROUND:
		outputPan[0] = left;
		outputPan[3] = right;
		break;

	case SPEAKER_QUAD:
		outputPan[0] = outputPan[2] = left;
		outputPan[1] = outputPan[3] = right;
		break;

	case SPEAKER_4POINT1:
		outputPan[0] = outputPan[3] = left;
		outputPan[1] = outputPan[4] = right;
		break;

	case SPEAKER_5POINT1:
	case SPEAKER_7POINT1:
	case SPEAKER_5POINT1_SURROUND:
		outputPan[0] = outputPan[4] = left;
		outputPan[1] = outputPan[5] = right;
		break;

	case SPEAKER_7POINT1_SURROUND:
		outputPan[0] = outputPan[4] = outputPan[6] = left;
		outputPan[1] = outputPan[5] = outputPan[7] = right;
		break;
	}

	sourceVoice_->SetOutputMatrix(Audio::Instance().GetMasteringVoice(), voiceDetails.InputChannels, masterDetails.InputChannels, outputPan);

}

void AudioSource::Filter(XAUDIO2_FILTER_TYPE type, FLOAT32 cutoff, FLOAT32 overq)
{
	filterParameters_.Type = type; //使うフィルターの種類
	filterParameters_.Frequency										//カットする周波数の基準(0Hz(0.0f) ~ 7350Hz(1.0f))
		= cutoff / wfe_.nSamplesPerSec * 6.0f; //式:カットオフ周波数 / サンプリングレート * 6.0f 例 : 7350 / 44100 * 6.0f = 1.0f(正確には1.000002fだがまあこれでおｋ)
																	//単極フィルターを使う場合はXAudio2CutoffFrequencyToOnePoleCoefficient()というマクロを使う(XAUDIO2_HELPER_FUNCTIONSが必要)
				 
	filterParameters_.OneOverQ = overq; //実際にどのくらいの音量がカットされているかを指定する

	sourceVoice_->SetFilterParameters(&filterParameters_);

}

void AudioSource::DrawDebug()
{
#ifdef USE_IMGUI
#endif
}
