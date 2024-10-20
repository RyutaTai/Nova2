#pragma once

#include "Bullet.h"

class BulletManager;

//	直進弾丸
class BulletStraight :public Bullet
{
public:
	BulletStraight(const std::string& fileName);
	~BulletStraight()override;

	//	更新処理
	void Update(const float& elapsedTime)override;

	//	描画処理
	void Render()override;
	void RnederCoverModel()override {}

	//	発射
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	float				speed_ = 300.0f;		//	球の速さ
	float				lifeTimer_ = 3.0f;		//	球の生存時間

};