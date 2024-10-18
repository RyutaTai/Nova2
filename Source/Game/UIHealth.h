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

	void UpdateHpGauge(const float& elpasedTime);		//	HP�Q�[�W�X�V
	void UpdateHpGaugeBack(const float& elpasedTime);	//	����HP�Q�[�W�X�V
	void CheckDamage();									//	�_���[�W��H��������m�F����


private:
	std::unique_ptr<Sprite> hpFrame_;
	std::unique_ptr<Sprite> hpGaugeBack_;

	const float GAUGE_SIZE_X = 440.0f;		//	HP�Q�[�W�̉摜�T�C�YX

	float oldHealth_ = 0.0f;				//	�O���HP
	bool isUpdateHpGaugeBack_ = false;		//	���̃Q�[�W�X�V�t���O
	float startValue_ = 0.0f;				//	�⊮�J�n
	float endValue_ = 0.0f;					//	�⊮�I��
	float gaugeLerpTimer_ = 0.0f;			//	�⊮�ɂ����鎞��
	float gaugeLerpSpeed_ = 1.0f;			//	�⊮���鑬��

};

