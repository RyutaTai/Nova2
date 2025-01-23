#pragma once

#include "Bullet.h"

class BulletManager;

//	’¼i’eŠÛ
class BulletStraight :public Bullet
{
public:
	BulletStraight(const std::string& filename);
	~BulletStraight()override;

	void Initialize()override;
	void Update(const float& elapsedTime)override;

	//	----- •`‰æˆ— -----
	void Render()override;
	void RnederCoverModel()override {}

	//	----- Collision -----
	void RegisterCollisionData()override;
	void UpdateCollisions(const float& elapsedTime)override;

	//	”­Ë
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)override;

private:
	float				speed_ = 300.0f;		//	‹…‚Ì‘¬‚³
	float				lifeTimer_ = 3.0f;		//	‹…‚Ì¶‘¶ŠÔ

};