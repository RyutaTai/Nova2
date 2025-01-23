#include "BulletStraight.h"

//	コンストラクタ
BulletStraight::BulletStraight(const std::string& filename)
	:Bullet(filename)
{
	//	スケール設定
	GetTransform()->SetScaleFactor(30.0f);
}

//	デストラクタ
BulletStraight::~BulletStraight()
{
	
}

//	初期化処理
void BulletStraight::Initialize()
{

}

//	当たり判定登録
void BulletStraight::RegisterCollisionData()
{

}

//	当たり判定更新
void BulletStraight::UpdateCollisions(const float& elapsedTime)
{

}

//	更新処理
void BulletStraight::Update(const float& elapsedTime)
{
	Bullet::Update(elapsedTime);

	//	寿命処理
	lifeTimer_ -=elapsedTime;
	if (lifeTimer_ <= 0.0f)
	{
		//	自分を削除
		Destroy(elapsedTime);
	}

	//	移動
	float speed = this->speed_ * elapsedTime;
	DirectX::XMFLOAT3 position = GetTransform()->GetPosition();
	position.x += direction_.x * speed;
	position.y += direction_.y * speed;
	position.z += direction_.z * speed;
	GetTransform()->SetPosition(position);

}

//	発射
void BulletStraight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	Bullet::Launch();

	this->direction_ = direction;
	this->GetTransform()->SetPosition(position);
}

//	描画処理
void BulletStraight::Render()
{
	gltfStaticModelResource_->Render();
}
