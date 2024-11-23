#include "AudioManager.h"

#include "../Others/Misc.h"
#include "../../imgui/imgui.h"

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
	Finalize();

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
AudioSource* AudioManager::LoadAudioSource(const char* filename)
{
	WaveReader* resource = new WaveReader(filename);
	return new AudioSource(xaudio_, resource);
}

AudioSource3D* AudioManager::LoadAudioSource3D(const char* filename, SoundEmitter* emitter)
{
	auto resource = new WaveReader(filename);
	return new AudioSource3D(xaudio_, resource, emitter);
}

//	更新処理
void AudioManager::Update(const float& elapsedTime)
{
	//	破棄処理
	for (AudioSource* audio : audioRemoves_)
	{
		if (audio->IsSE() && audio->GetState().BuffersQueued != 0)continue;	//	SEかつオーディオ再生が終了していなかったら破棄しない

		std::vector<AudioSource*>::iterator it =
			std::find(audioResources_.begin(), audioResources_.end(), audio);

		if (it != audioResources_.end())
		{
			audioResources_.erase(it);
		}

		delete audio;
	}

	//	破棄リストをクリア
	audioRemoves_.clear();

	for (AudioSource* audio : audioResources_)
	{
		audio->Update(elapsedTime);
	}
}

//	オーディオ登録
void AudioManager::Register(AudioSource* audio)
{	
	audioResources_.emplace_back(audio);
}

//	オーディオを名前から取得(例: Title.wavなど)
AudioSource* AudioManager::GetAudioResource(const std::string& name)
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

//	オーディオ削除
void AudioManager::Remove(AudioSource* audio)
{
	audioRemoves_.insert(audio);
}

//	シーンを指定してオーディオ削除
void AudioManager::RemoveByScene(const std::string& sceneName)
{
	for (AudioSource* audio : audioResources_)
	{
		if (strcmp(audio->GetSceneName().c_str(), sceneName.c_str()) == 0)	//	オーディオデータのシーンと一致したら
		{
			audioRemoves_.insert(audio);
		}
	}
}

//	オーディオ全削除
void AudioManager::Clear()
{
	for (AudioSource*& audio : audioResources_)
	{
		delete audio;
	}
	audioResources_.clear();
}

//	オーディオ終了化
void AudioManager::Finalize()
{
	Clear();
}

//	デバッグ描画
void AudioManager::DrawDebug()
{
	if (ImGui::TreeNode("AudioManager"))
	{
		int size = audioResources_.size();
		ImGui::DragInt("AudioCount", &size);	//	オーディオの数
		ImGui::TreePop();
	}
}