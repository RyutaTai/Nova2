#include "UIInstructions.h"

#include "../../External/imgui/imgui.h"

UIInstructions::UIInstructions()
	:UI()
{
	instruction_ = std::make_unique<Sprite>(L"./Resources/Image/Instructions.png");
	instruction_->GetTransform()->SetPosition(24, 250);
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