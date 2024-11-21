#include "UITempo.h"

#include "../../External/imgui/imgui.h"
#include "../Nova/Input/Input.h"

UITempo::UITempo()
	:UI()
{
	//	真ん中の円
	center_ = std::make_unique<Sprite>(L"./Resources/Image/TempoUI.png");
	center_->GetTransform()->CutOutX(-300.0f);
	center_->GetTransform()->SetTexPosX(100.0f);
	center_->GetTransform()->SetPosition(912, 855);

	//	両サイドの半円



	//	描画フラグをfalseにしておく。ビューボタンを押したら描画する
	SetDrawFlag(false);
}

//	初期化処理
void UITempo::Initialize()
{

}

//	更新処理
void UITempo::Update(const float& elapsedTime)
{
	UpdateDrawFlag();
}

//	描画フラグ切り替え処理
void UITempo::UpdateDrawFlag()
{
	bool drawFlag = GetDrawFlag();
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_BACK)
	{
		SetDrawFlag(!drawFlag);
	}
}

//	UIの位置更新処理
void UITempo::UpdatePosition(const float& elapsedTime)
{

}

//	UIのスケール更新処理
void UITempo::UpdateScale(const float& elapsedTime)
{

}

void UITempo::Render()
{
	center_->Render();
}

void UITempo::DrawDebug()
{
	if (ImGui::TreeNode("Tempo"))
	{
		ImGui::Text("Center");
		center_->DrawDebug();
		ImGui::TreePop();
	}
}