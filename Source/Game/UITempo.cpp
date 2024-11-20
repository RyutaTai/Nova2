#include "UITempo.h"

#include "../../External/imgui/imgui.h"
#include "../Nova/Input/Input.h"

UITempo::UITempo()
	:UI()
{
	center_ = std::make_unique<Sprite>(L"./Resources/Image/TempoUI.png");
	center_->GetTransform()->CutOutX(-100.0f);
	//center_->GetTransform()->CutOut();
	center_->GetTransform()->SetPosition(480, 600);

	//	描画フラグをfalseにしておく。ビューボタンを押したら描画する
	SetDrawFlag(false);
}

void UITempo::Initialize()
{

}

void UITempo::Update(const float& elapsedTime)
{
	UpdateDrawFlag();
}

void UITempo::UpdateDrawFlag()
{
	bool drawFlag = GetDrawFlag();
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_BACK)
	{
		SetDrawFlag(!drawFlag);
	}
}

void UITempo::Render()
{
	center_->Render();
}

void UITempo::DrawDebug()
{
	if (ImGui::TreeNode("Tempo"))
	{
		ImGui::TreePop();
	}
}