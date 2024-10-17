#pragma once

#include "UI.h"

class UIHealth : public UI
{
public:
	UIHealth();
	~UIHealth(){}

	void Initialize();
	void Update(const float& elapsedTime)override;
	void Render();
	void DrawDebug();

	void UpdateHpGauge(const float& elpasedTime);		//	Hpゲージ更新
	void UpdateHpGaugeBack(const float& elpasedTime);	//	後ろのHpゲージ更新

	void SetOldHealth(const float& health);

	float XMFloatLerp(const float& start, const float& end, const float& timer);

private:
	float amountOfDecrease_ = 0.0f;	//	減少量
	std::unique_ptr<UI> hpFrame_;
	std::unique_ptr<UI> hpGaugeBack_;

	float oldHealth_;
	float currentHealth_;
	bool isUpdateHealth_ = false;
	float minHp_;
	float maxHp_;

	float gaugeLerpTimer_ = 0.0f;	//	ゲージの補完に使用するタイマー
	float backLerpTimer_ = 0.0f;	//	後ろのゲージの補完に使用するタイマー

	const float GAUGE_SIZE_X = 440.0f;	//	HPゲージの画像サイズX
	const float GAUGE_SIZE_Y = 20.0f;	//	HPゲージの画像サイズY
};

