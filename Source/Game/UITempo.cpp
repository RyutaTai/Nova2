#include "UITempo.h"

#include "../../External/imgui/imgui.h"

UITempo::UITempo()
	:UI()
{
	center_ = std::make_unique<Sprite>(L"./Resources/Image/TempoUI.png");
	center_->GetTransform()->SetPosition(480, 680);
}

void UITempo::Initialize()
{

}

void UITempo::Update(const float& elapsedTime)
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
		ImGui::TreePop();
	}
}