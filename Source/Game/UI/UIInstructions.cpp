#include "UIInstructions.h"

#include "../../../External/imgui/imgui.h"

UIInstructions::UIInstructions()
	:UI()
{
	instruction_ = std::make_unique<Sprite>(L"./Resources/Image/Instructions.png");
	instruction_->GetTransform()->SetPosition(24, 250);
	instruction_->GetTransform()->SetIsCut(true);
	instruction_->GetTransform()->SetCutSizeY(200.0f);
}

void UIInstructions::Initialize()
{

}

void UIInstructions::Update(const float& elapsedTime)
{

}

void UIInstructions::Render()
{
	instruction_->Render();
}

void UIInstructions::DrawDebug()
{
	if (ImGui::TreeNode("Instructions"))
	{
		ImGui::TreePop();
	}
}