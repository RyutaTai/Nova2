#pragma once

#include <memory>

#include "Bullet.h"
#include "../Nova/Resources/Effect.h"

class BulletHorming :public Bullet
{
public:
	BulletHorming(const std::string& fileName);
	~BulletHorming()override;

	//	更新処理
	void Update(const float& elapsedTime)override;

	//	描画処理
	void Render()override;
	void RnederCoverModel()override;

	void Move(const float& elpasedTime);													//	移動処理
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);		//	発射
	void Destroy(const float& elapsedTime)override;											//	破棄処理
	void SetTarget(const DirectX::XMFLOAT3& target) { target_ = target; }					//	ターゲット設定

	void DrawDebug();	//	デバッグ描画

private:
	enum EFFECT
	{
		FIRE = 0,		//	弾の周りのエフェクト
		EXPLOSION,		//	爆発
		MAX,			//	最大数
	};

private:
	float							speed_ = 200.0f;						//	弾の速さ
	//float							speed_		= 1.0f;						//	弾の速さ
	float							lifeTimer_ = 3.0f;						//	弾の生存時間
	DirectX::XMFLOAT3				target_ = { 0,0,0 };					//	ターゲット位置
	std::shared_ptr <Effect>		effectResource_[EFFECT::MAX];			//	エフェクト		
	float							effectScale_[EFFECT::MAX] = { 1.0f };	//	エフェクトスケール

};

