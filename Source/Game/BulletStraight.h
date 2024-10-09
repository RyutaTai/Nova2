#pragma once

#include "Bullet.h"

class BulletManager;

//	’¼i’eŠÛ
class BulletStraight :public Bullet
{
public:
	BulletStraight(const std::string& fileName);
	~BulletStraight()override;

	//	XVˆ—
	void Update(const float& elapsedTime)override;

	//	•`‰æˆ—
	void Render()override;
	void RnederCoverModel()override {}

	//	”­Ë
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	float				speed_ = 300.0f;		//	‹…‚Ì‘¬‚³
	float				lifeTimer_ = 3.0f;		//	‹…‚Ì¶‘¶ŠÔ

};