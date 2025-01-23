#pragma once

#include <memory>

#include "Bullet.h"
#include "../Nova/Resources/Effect.h"

class BulletHorming :public Bullet
{
public:
	BulletHorming(const std::string& filename);
	~BulletHorming()override;

	void Initialize()override;
	void Update(const float& elapsedTime)override;
	
	//	----- 描画処理 -----
	void Render()override;
	void RnederCoverModel()override;

	//	----- デバッグ描画 -----
	void DrawDebug();
	
	//	----- Collision -----
	void RegisterCollisionData()override;
	void UpdateCollisions(const float& elapsedTime)override;

	void Move(const float& elpasedTime);														//	移動処理
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)override;	//	発射
	void Destroy(const float& elapsedTime)override;												//	破棄処理
	void SetTarget(const DirectX::XMFLOAT3& target) { target_ = target; }						//	ターゲット設定


private:
	enum EFFECT
	{
		FIRE = 0,		//	弾の周りのエフェクト
		EXPLOSION,		//	爆発
		Max,			//	最大数
	};

private:
	float							speed_ = 1.0f;						//	弾の速さ
	//float							speed_		= 1.0f;						//	弾の速さ
	float							lifeTimer_ = 2.5f;						//	弾の生存時間
	DirectX::XMFLOAT3				target_ = { 0,0,0 };					//	ターゲット位置
	std::shared_ptr <Effect>		effectResource_[EFFECT::Max];			//	エフェクト		
	float							effectScale_[EFFECT::Max] = { 1.0f };	//	エフェクトスケール

};

