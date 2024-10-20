#pragma once

#include <vector>
#include <set>

#include "../Game/Bullet.h"

class Bullet;

class BulletManager
{
public:
	BulletManager();
	~BulletManager();

	static BulletManager& Instance()
	{
		static BulletManager instance;
		return instance;
	}

	//	初期化処理
	void Initialize();

	//	更新処理
	void Update(const float& elapsedTime);

	//	カバーモデルスケール更新処理
	void CoverModelUpdate(const float& elapsedTime);

	//	描画処理
	void Render();

	//	弾丸登録
	void Register(Bullet* bullet);

	//	弾丸全削除
	void Clear();

	//	カバーモデルスケールセット
	void SetCoverScale(float& coverScale) { coverScale_ = coverScale; }

	//	カバーモデルスケール取得
	const float& GetCoverScale() const { return coverScale_; }

	//	弾丸数取得
	int GetBulletCount()const { return static_cast<int>(bullets_.size()); }

	//	弾丸取得
	Bullet* GetBullet(int index) { return bullets_.at(index); }

	//	弾丸削除
	void Remove(Bullet* projectile);

	//	デバッグプリミティブ描画
	void DrawDebugPrimitive();

	//	デバッグ描画
	void DrawDebug();

private:
	std::vector	<Bullet*>	bullets_;
	std::set	<Bullet*>	removes_;
#if 0
	float					coverScale_		= 30.0f;	//	カバーモデルのスケール
	float					scaleAdd_		= 10.0f;	//	どのくらいscaleを加算するか
	const float				COVER_SCALE_MAX = 50.0f;	//	スケール上限
	const float				COVER_SCALE_MIN = 20.0f;	//	スケール下限	小さすぎると弾に埋まって見えない
#else
	//float					coverScale_ = 10;		//	カバーモデルのスケール
	//float					scaleAdd_ = 5;			//	どのくらいscaleを加算するか
	//const float				COVER_SCALE_MAX = 12;	//	スケール上限
	//const float				COVER_SCALE_MIN = 7;	//	スケール下限	小さすぎると弾に埋まって見えない

	float					coverScale_ = 0.1f;		//	カバーモデルのスケール
	float					scaleAdd_ = 1.0f;		//	どのくらいscaleを加算するか
	const float				COVER_SCALE_MAX = 1.2f;	//	スケール上限
	const float				COVER_SCALE_MIN = 0.7f;	//	スケール下限	小さすぎると弾に埋まって見えない

	float					coverScaleTimer_ = 0.0f;

#endif

private:	//	デバッグ用の変数
	bool isBulletDestroy_ = false;		//	弾丸破棄
	bool coverModelDraw_ = true;		//	カバーモデル描画フラグ
	bool isCoverModelUpdate_ = true;	//	カバーモデル更新処理

};