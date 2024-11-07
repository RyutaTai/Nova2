#include "AudioManager.h"

#include "../Others/Misc.h"

void AudioManager::Initialize()
{
	HRESULT hr = S_OK;

	//	COMの初期化
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	UINT32 createFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createFlags |= XAUDIO2_DEBUG_ENGINE;
#endif

	//	XAudio初期化
	hr = XAudio2Create(&xaudio_, createFlags);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	//	create masteringvoice
#if 0 //	自動検出
	hr = xaudio->CreateMasteringVoice(&masteringVoice, XAUDIO2_DEFAULT_CHANNELS, 44100/*サンプリングレート*/, 0U, NULL, 0, AudioCategory_GameEffects);
#else
	hr = xaudio_->CreateMasteringVoice(&masteringVoice_, 2, 44100/*サンプリングレート*/, 0U, NULL, 0, AudioCategory_GameEffects);
#endif

	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	masteringVoice_->GetChannelMask(&channelMask_);

}

//	デストラクタ
AudioManager::~AudioManager()
{
	//	オーディオ全削除
	Clear();

	//	マスタリングボイス破棄
	if (masteringVoice_ != nullptr)
	{
		masteringVoice_->DestroyVoice();
		masteringVoice_ = nullptr;
	}

	//	XAudio終了化
	if (xaudio_ != nullptr)
	{
		xaudio_->Release();
		xaudio_ = nullptr;
	}

	//	COM終了化
	CoUninitialize();
}

//	オーディオソース読み込み
std::shared_ptr<AudioSource> AudioManager::LoadAudioSource(const char* filename)
{
	std::shared_ptr<WaveReader> resource = std::make_shared<WaveReader>(filename);
	return std::make_shared<AudioSource>(xaudio_, resource);
}

std::shared_ptr<AudioSource3D> AudioManager::LoadAudioSource3D(const char* filename, SoundEmitter* emitter)
{
	auto resource = std::make_shared<WaveReader>(filename);
	return std::make_shared<AudioSource3D>(xaudio_, resource, emitter);
}

//	更新処理
void AudioManager::Update(const float& elapsedTime)
{
	for (std::shared_ptr<AudioSource> audio : audioResources_)
	{
		audio->Update(elapsedTime);
	}
}

//	オーディオ登録
void AudioManager::Register(std::shared_ptr<AudioSource> audio)
{
	audioResources_.emplace_back(audio);
}

//	オーディオを名前から取得(例: Title.wavなど)
std::shared_ptr<AudioSource> AudioManager::GetAudioResource(const std::string& name)
{
	for (int i = 0; i < audioResources_.size(); ++i)
	{
		if (strcmp(audioResources_.at(i)->GetName().c_str(), name.c_str()) == 0)	//	入力文字列と等しいデータがあれば
		{
			return audioResources_.at(i);
		}
	}
	_ASSERT_EXPR(false, L"Audio is not found.");
	return nullptr;
}

void AudioManager::Remove(AudioSource* audio)
{

}

//	オーディオ全削除
void AudioManager::Clear()
{
	for (std::shared_ptr<AudioSource> audio : audioResources_)
	{
		audio.reset();
	}
	audioResources_.clear();
}

void AudioManager::DrawDebug()
{

}