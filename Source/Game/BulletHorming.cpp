#include "BulletHorming.h"

#include "../Nova/Others/MathHelper.h"
#include "../Nova/Core/Framework.h"
#include "Player.h"
#include "../Nova/Collision/Collision.h"

//	コンストラクタ
BulletHorming::BulletHorming(const std::string& filename)
	:Bullet(filename)
{
	//	エフェクト読み込み
	effectResource_[EFFECT::EXPLOSION] = ResourceManager::Instance().LoadEffectResource("./Resources/Effect/Blow11_2.efk");
	effectScale_[EFFECT::EXPLOSION] = 0.3f;

	//	カバーモデル
	DirectX::XMFLOAT4 coverModelColor = { 1.0f,0.0f,0.0f,1.0f };
	coverModel_ = std::make_unique<GltfModelStaticBatching>("./Resources/Model/Cube/source/Cube2.gltf", true, coverModelColor);
	
	//	透明処理
#if 1
	//coverModel_->GetTransform()->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
#else
	coverModel_->GetTransform()->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
#endif
	//	移動速度設定
	speed_ = 6.0f;

}

//	デストラクタ
BulletHorming::~BulletHorming()
{

}

//	更新処理
void BulletHorming::Update(const float& elapsedTime)
{
	//	移動処理
	Move(elapsedTime);

	//	破棄処理
	Destroy(elapsedTime);

	//	カバーモデル更新処理
	CoverModelUpdate(elapsedTime);

}

//	発射
void BulletHorming::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	this->direction_ = direction;
	this->GetTransform()->SetPosition(position);

}

//	破棄処理
void BulletHorming::Destroy(const float& elapsedTime)
{
	//	寿命処理
	lifeTimer_ -= elapsedTime;

	//	球と円柱の当たり判定
	bool isHitPlayer = false;
	DirectX::XMFLOAT3 bulletPos = this->GetTransform()->GetPosition();
	DirectX::XMFLOAT3 playerPos = Player::Instance().GetTransform()->GetPosition();		//	プレイヤーの位置
	float playerRadius = Player::Instance().GetRadius();								//	プレイヤーの半径
	float playerHeight = Player::Instance().GetHeight();								//	プレイヤーの高さ
	playerPos.y += playerHeight / 2;
	DirectX::XMFLOAT3 outPosition = {};
	isHitPlayer = Collision::IntersectSphereVsCylinder(bulletPos, radius_, playerPos, playerRadius, playerHeight, outPosition);

	//	生存時間がなくなるか、プレイヤーに当たるか、プレイヤーに攻撃されたら
#if 1
	if (lifeTimer_ <= 0.0f || isHitPlayer || damaged_)
#else
	if(damaged_)
#endif
	{
		DirectX::XMFLOAT3 pos = GetTransform()->GetPosition();

		//	エフェクト描画
		effectResource_[EFFECT::EXPLOSION]->Play(pos, effectScale_[EFFECT::EXPLOSION]);
		//effectResource_[EFFECT::EXPLOSION]->PlayAsync(pos, effectScale_[EFFECT::EXPLOSION]);

		//	自分を削除
		Bullet::Destroy(elapsedTime);

	}

	if (isHitPlayer)
	{
		Player::Instance().SubtractHp(10);
	}

}

//	移動処理
void BulletHorming::Move(const float& elapsedTime)
{
	//	移動
	DirectX::XMFLOAT3 myPos = GetTransform()->GetPosition();
	DirectX::XMFLOAT3 dir;
	target_ = Player::Instance().GetTransform()->GetPosition();
	target_.y += Player::Instance().GetHeight() / 1.5f;			//	プレイヤーの拳に当たるようにするため
	dir = target_ - myPos;
	dir = Normalize(dir);										//	正規化

	float speed = speed_ * elapsedTime;
	DirectX::XMFLOAT3 position = GetTransform()->GetPosition();

	velocity_ = dir * speed;

	position = position + velocity_;

	GetTransform()->SetPosition(position);

}

//	描画処理
void BulletHorming::Render()
{
	//	弾丸モデル描画
	gltfStaticModelResource_->Render();

}

//	カバーモデル描画
void BulletHorming::RnederCoverModel()
{
	float coverScale = BulletManager::Instance().GetCoverScale();
	Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ADD);
	//Graphics::Instance().GetShader()->CreatePsFromCso(Graphics::Instance().GetDevice(), "./Resources/Shader/BulletCoverPS.cso", pixelShader_.ReleaseAndGetAddressOf());
	//coverModel_->SetPixelShader(coverPixelShader_.Get());
	
	//coverModel_->SetPixelShader("./Resources/Shader/GltfModelPS.cso");
	
	coverModel_->SetPixelShaderFromName("./Resources/Shader/BulletCoverPS.cso");
	coverModel_->GetTransform()->SetScaleFactor(coverScale);
	coverModel_->Render();
}

//	デバッグ描画
void BulletHorming::DrawDebug()
{
	if (ImGui::TreeNode(u8"Bullet 弾丸"))
	{
		GetTransform()->DrawDebug();
		ImGui::DragFloat("Radius", &radius_, 1.0f, -FLT_MAX, FLT_MAX);				//	半径
		ImGui::DragFloat3("Target", &target_.x, 1.0f, -FLT_MAX, FLT_MAX);			//	ターゲット
		ImGui::DragFloat3("OwnerPos", &ownerPosition_.x, 0.1f, -FLT_MAX, FLT_MAX);	//	所有者の位置
		ImGui::DragFloat("Speed", &speed_, 0.5f, -FLT_MAX, FLT_MAX);				//	弾の速さ
		ImGui::DragFloat("LifeTimer", &lifeTimer_, 0.5f, -FLT_MAX, FLT_MAX);		//	生存時間
		ImGui::TreePop();
	}
}