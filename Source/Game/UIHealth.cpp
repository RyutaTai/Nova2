#include "UIHealth.h"

#include "Player.h"

UIHealth::UIHealth()
	:UI::UI("./Resources/Image/HpGauge.png")
{
	hpGaugeBack_ = std::make_unique<UI>("./Resources/Image/HpGaugeBack.png");
	hpFrame_ = std::make_unique<UI>("./Resources/Image/HpFrame.png");

	this->GetTransform()->SetPosition(80, 40);
	this->SetName("HpGauge");
	hpGaugeBack_->GetTransform()->SetPosition(88, 41);
	hpGaugeBack_->SetName("HpGaugeBack");
	hpFrame_->GetTransform()->SetPosition(0, 0);
	hpFrame_->SetName("HpFrame");

	oldHealth_ = 100.0f;

}

void UIHealth::Initialize()
{
	
}

void UIHealth::Update(const float& elapsedTime)
{
	UpdateHpGauge(elapsedTime);
	UpdateHpGaugeBack(elapsedTime);


	//const float hp = XMFloatLerp(maxHp_, minHp_, gaugeLerpTimer_);
	int hp = Player::Instance().GetHp();
	SetOldHealth(hp);

	//int maxHp = Player::Instance().GetMaxHp();
	//int hp = Player::Instance().GetHp();
	//int sizeX = ui_[static_cast<int>(UI_GAME::HpGauge)]->GetTransform()->GetSizeX();
	//int amountOfDecrease = (maxHp - hp) * (sizeX / maxHp);
	
	//ui_[static_cast<int>(UI_GAME::HpGauge)]->GetTransform()->SetCutSizeX(amountOfDecrease);
	//ui_[static_cast<int>(UI_GAME::HpGauge)]->GetTransform()->CutOut();
	//ui_[static_cast<int>(UI_GAME::HpGauge)]->GetTransform()->CutOutX(amountOfDecrease);

}

//	HPゲージ更新
void UIHealth::UpdateHpGauge(const float& elapsedTime)
{
	if (isUpdateHealth_)
	{
		gaugeLerpTimer_ += elapsedTime * 4.4f;
		gaugeLerpTimer_ = min(gaugeLerpTimer_, 1.0f);

		//const float hp = XMFloatLerp(maxHp_, minHp_, gaugeLerpTimer_);
		//SetOldHealth(hp);
		//Player::Instance().SetHp(hp);

		//this->GetTransform()->SetCutSizeX(-1.3f);
		//this->GetTransform()->CutOut();
		//this->GetTransform()->CutOutX(-1.3f);

		const int PLAYER_MAX_HP = Player::Instance().GetMaxHp();
		int amountOfDecrease = GAUGE_SIZE_X * (maxHp_ - minHp_) / PLAYER_MAX_HP;	//	HPゲージ減少量

		hpGaugeBack_->GetTransform()->CutOutX(-amountOfDecrease);
		isUpdateHealth_ = false;
		if (gaugeLerpTimer_ == 1.0f) isUpdateHealth_ = false;


	}
}

//	後ろのHPゲージ更新
void UIHealth::UpdateHpGaugeBack(const float& elapsedTime)
{
	if (isUpdateHealth_)
	{
		backLerpTimer_ += elapsedTime * 3.0f;
		backLerpTimer_ = min(backLerpTimer_, 1.0f);

		//const float hp = XMFloatLerp(maxHp_, minHp_, backLerpTimer_);
		//Player::Instance().SetHp(hp);

		//hpGaugeBack_->GetTransform()->SetCutSizeX(-1.3f);
		//hpGaugeBack_->GetTransform()->CutOut();
		//hpGaugeBack_->GetTransform()->CutOutX(-1.3f);
		
		const int PLAYER_MAX_HP = Player::Instance().GetMaxHp();
		int amountOfDecrease = GAUGE_SIZE_X * (maxHp_ - minHp_) / PLAYER_MAX_HP;	//	HPゲージ減少量
		
		hpGaugeBack_->GetTransform()->CutOutX(-amountOfDecrease);
		isUpdateHealth_ = false;
		if (backLerpTimer_ == 1.0f) isUpdateHealth_ = false;


	}
}

void UIHealth::SetOldHealth(const float& health)
{
	currentHealth_ = health;
	if (currentHealth_ != oldHealth_)
	{
		isUpdateHealth_ = true;

		minHp_ = health;
		maxHp_ = oldHealth_;

		gaugeLerpTimer_ = 0.0f;
		backLerpTimer_ = 0.0f;
	}
	oldHealth_ = health;
}

float UIHealth::XMFloatLerp(const float& start, const float& end, const float& timer)
{
	return start + timer / (end - start);
}

void UIHealth::Render()
{
	UI::Render();
	hpGaugeBack_->Render();
	hpFrame_->Render();
}

void UIHealth::DrawDebug()
{

}