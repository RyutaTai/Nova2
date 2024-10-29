#include "UITempo.h"

#include "../../External/imgui/imgui.h"

UITempo::UITempo()
	:UI()
{
	tempo_ = std::make_unique<Sprite>(L"./Resources/Image/TempoUI.png");
	tempo_->GetTransform()->SetPosition(480, 680);
}

void UITempo::Initialize()
{

}

void UITempo::Update(const float& elapsedTime)
{

}

void UITempo::Render()
{
	tempo_->Render();
}

void UITempo::DrawDebug()
{
	if (ImGui::TreeNode("Tempo"))
	{
		ImGui::TreePop();
	}
}