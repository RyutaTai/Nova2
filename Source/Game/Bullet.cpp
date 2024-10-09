#include "Bullet.h"

#include "../Nova/Debug/DebugRenderer.h"
#include "../Nova/Graphics/Graphics.h"
#include "../Nova/Collision/Collision.h"
#include "../Nova/Audio/Audio.h"

//	コンストラクタ
Bullet::Bullet(const std::string& fileName)
	:GameStaticObject(fileName)
{
	//	生成時にマネージャーに登録する
	BulletManager::Instance().Register(this);

	//	弾丸スケール設定(描画用)
	float scale = 60.0f;
	GetTransform()->SetScaleFactor(scale);

	//	弾丸半径(当たり判定用)
	radius_ = 1.0f;

	//	SE読み込み
	emitter_ = std::make_unique<SoundEmitter>();
	emitter_->position = GetTransform()->GetPosition();
	emitter_->velocity = velocity_;
	emitter_->min_distance = 7.0f;
	emitter_->max_distance = 12.0f;
	emitter_->volume_ = 1.0f;
	//se_[static_cast<int>(AUDIO_SE_BULLET::Explosion)] = std::unique_ptr<AudioSource3D>(Audio::Instance().LoadAudioSource3D("./Resources/Audio/SE/GameStart_015.wav", emitter_.get()));

}

//	初期化処理
void Bullet::Initialize()
{

}

//	更新処理
void Bullet::Update(const float& elapsedTime)
{

}

//	カバーモデル更新処理
void Bullet::CoverModelUpdate(const float& elpasedTime)
{
	//	位置更新
	DirectX::XMFLOAT3 bulletPos = this->GetTransform()->GetPosition();
	coverModel_->GetTransform()->SetPosition(bulletPos);
}

//	破棄
void Bullet::Destroy(const float& elapsedTime)
{
	//	爆発音再生
	//se_[static_cast<int>(AUDIO_SE_BULLET::Explosion)]->Play(false);

	//	マネージャーから自分を削除する
	BulletManager::Instance().Remove(this);
}

//	デバッグプリミティブ描画
void Bullet::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//	衝突判定用のデバッグ球を描画
	DirectX::XMFLOAT3 position = this->GetTransform()->GetPosition();
	debugRenderer->DrawSphere(position, radius_, DirectX::XMFLOAT4(0, 0, 0, 1));
}

//	デバッグ描画
void Bullet::DrawDebug()
{
	float scale = GetTransform()->GetScaleFactor();

	if (ImGui::TreeNode(u8"Bullet 弾丸"))
	{
		GetTransform()->DrawDebug();
		ImGui::DragFloat("Radius", &radius_, 1.0f, -FLT_MAX, FLT_MAX);	//	半径
		ImGui::DragFloat("Scale", &scale, 0.1f, 1.0f, FLT_MAX);			//	スケール
		ImGui::TreePop();
	}

	GetTransform()->SetScaleFactor(scale);

}