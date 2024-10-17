#include "UI.h"

#include "../../imgui/imgui.h"
#include "../Nova/Resources/ResourceManager.h"
#include "UIManager.h"

UI::UI(const std::string& filename)
{
	//	�X�v���C�g�ǂݍ���
	spriteResource_ = ResourceManager::Instance().LoadSpriteResource(filename);

	//	�������Ƀ}�l�[�W���[�ɓo�^
	UIManager::Instance().Register(this);

}

void UI::Initialize()
{

}

void UI::Update(const float& elapsedTime)
{

}

void UI::Render()
{
	if (renderFlag_)
	{
		if (spriteResource_ != nullptr)spriteResource_->Render();
	}
}

//	�f�o�b�O�`��
void UI::DrawDebug()
{
	if (name_ == "")
	{
		name_ = "UI Name";
	}

	if (ImGui::TreeNode(name_.c_str()))
	{
		spriteResource_->GetTransform()->DrawDebug();
		ImGui::TreePop();
	}
}