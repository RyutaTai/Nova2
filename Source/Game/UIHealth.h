#pragma once

#include "UI.h"

class UIHealth : public UI
{
public:
	UIHealth();
	~UIHealth(){}

	void Initialize()override;
	void Update(const float& elapsedTime)override;
	void Render()override;
	void DrawDebug()override;

	void UpdateHpGauge(const float& elpasedTime);		//	Hpゲージ更新
	void UpdateHpGaugeBack(const float& elpasedTime);	//	後ろのHpゲージ更新
	void CheckDamage();									//	ダメージを食らったか確認する


private:
	std::unique_ptr<Sprite> hpFrame_;
	std::unique_ptr<Sprite> hpGaugeBack_;

	const float GAUGE_SIZE_X = 440.0f;		//	HPゲージの画像サイズX

	float oldHealth_ = 0.0f;
	bool isUpdateHpGaugeBack_ = false;		//	後ろのゲージ更新フラグ
	float startValue_ = 0.0f;
	float endValue_ = 0.0f;
	float gaugeLerpTimer_ = 0.0f;
	float gaugeLerpSpeed_ = 1.0f;

};

