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

	void UpdateHpGauge(const float& elpasedTime);		//	Hp�Q�[�W�X�V
	void UpdateHpGaugeBack(const float& elpasedTime);	//	����Hp�Q�[�W�X�V

	void SetOldHealth(const float& health);

	float XMFloatLerp(const float& start, const float& end, const float& timer);

private:
	float amountOfDecrease_ = 0.0f;	//	������
	std::unique_ptr<UI> hpFrame_;
	std::unique_ptr<UI> hpGaugeBack_;

	float oldHealth_;
	float currentHealth_;
	bool isUpdateHealth_ = false;
	float minHp_;
	float maxHp_;

	float gaugeLerpTimer_ = 0.0f;	//	�Q�[�W�̕⊮�Ɏg�p����^�C�}�[
	float backLerpTimer_ = 0.0f;	//	���̃Q�[�W�̕⊮�Ɏg�p����^�C�}�[

	const float GAUGE_SIZE_X = 440.0f;	//	HP�Q�[�W�̉摜�T�C�YX
	const float GAUGE_SIZE_Y = 20.0f;	//	HP�Q�[�W�̉摜�T�C�YY
};

