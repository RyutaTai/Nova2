#include "BulletStraight.h"

//	コンストラクタ
BulletStraight::BulletStraight(const std::string& fileName)
	:Bullet(fileName)
{
	//	スケール設定
	GetTransform()->SetScaleFactor(30.0f);
}

//	デストラクタ
BulletStraight::~BulletStraight()
{
	
}

//	更新処理
void BulletStraight::Update(const float& elapsedTime)
{
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

//	描画処理
void BulletStraight::Render()
{
	GameStaticObject::Render();
}

//	発射
void BulletStraight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	this->direction_ = direction;
	this->GetTransform()->SetPosition(position);
}