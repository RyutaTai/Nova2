#include "Bullet.h"

#include "../Nova/Debug/DebugRenderer.h"
#include "../Nova/Graphics/Graphics.h"
#include "../Nova/Collision/Collision.h"
#include "../Nova/Audio/AudioManager.h"
#include "Player.h"

//	コンストラクタ
Bullet::Bullet(const std::string& filename)
{
	//	モデル生成
	gltfStaticModelResource_= ResourceManager::Instance().LoadGltfModelStaticResource(filename);

	//	生成時にマネージャーに登録する
	BulletManager::Instance().Register(this);

	//	弾丸半径(当たり判定用)
	radius_ = 0.5f;

	//	スケール
	GetTransform()->SetScaleFactor(0.4f);

	//	オーディオ初期設定
	emitter_.position_ = GetTransform()->GetPosition();
	emitter_.velocity_ = velocity_;
	emitter_.minDistance_ = 7.0f;
	emitter_.maxDistance_ = 12.0f;
	emitter_.volume_ = 1.0f;
	//se_[static_cast<int>(AudioSE3D::Explosion)] = std::unique_ptr<AudioSource3D>(Audio::Instance().LoadAudioSource3D("./Resources/Audio/SE/GameStart_015.wav", emitter_.get()));
	se_[static_cast<int>(AudioSE3D::Move)] = AudioManager::Instance().LoadAudioSource3D("./Resources/Audio/SE/bulletMove.wav", Audio::AudioType::SE3D, "GameScene", &emitter_);
	se_[static_cast<int>(AudioSE3D::Move)]->SetVolume(0.3f, false);
	se_[static_cast<int>(AudioSE3D::Move)]->SetAudioName("BulletMove");
	AudioManager::Instance().Register(se_[static_cast<int>(AudioSE3D::Move)]);

}

//	初期化処理
void Bullet::Initialize()
{

}

//	更新処理
void Bullet::Update(const float& elapsedTime)
{
	UpdateAudioSource(elapsedTime);
}

//	オーディオソース更新
void Bullet::UpdateAudioSource(const float& elapsedTime)
{
	if (se_[static_cast<int>(AudioSE3D::Move)])
	{
		se_[static_cast<int>(AudioSE3D::Move)]->SetDSPSetting(Player::Instance().GetListener());
	}
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
	//	無敵状態なら破棄しない
	if (isInvincible_)return;

	//	爆発音再生
	//se_[static_cast<int>(AudioSE3D::Explosion)]->Play(false);

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