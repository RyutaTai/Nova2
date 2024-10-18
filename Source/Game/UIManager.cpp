#include "UIManager.h"

#include "../../External/imgui/imgui.h"
#include "../Nova/Graphics/Graphics.h" 

void UIManager::Initialize()
{
	for (UI*& ui : userInterfaces_)
	{
		ui->Initialize();
	}
}

void UIManager::Update(const float& elapsedTime)
{
	for (UI* ui : generates_)
	{
		userInterfaces_.emplace_back(ui);
	}
	generates_.clear();

	for (UI*& ui : userInterfaces_)
	{
		ui->Update(elapsedTime);
	}
}

void UIManager::Register(UI* ui)
{
	generates_.insert(ui);
}

void UIManager::Finalize()
{
	for (UI*& ui : userInterfaces_)
	{
		delete ui;
	}
	userInterfaces_.clear();
}

void UIManager::SetDrawFlag(const bool& drawFlag)
{
	for (UI*& ui : userInterfaces_)
	{
		ui->SetRenderFlag(drawFlag);
	}
}

//	�ԍ�����UI���擾
UI* UIManager::GetUIFromNum(int num)
{
	//	num��userInterfaces_���傫��������A�T�[�g�ŗ��Ƃ�
	_ASSERT_EXPR(num < userInterfaces_.size(), L"UI num is too large.");

	return userInterfaces_.at(num);
}

//	���O����UI���擾
UI* UIManager::GetUIFromName(const std::string& name)
{
	for (UI*& ui : userInterfaces_)
	{
		if (ui->GetName().compare(name)) return ui;
	}
	_ASSERT_EXPR(false, L"UI is not found.");
}

void UIManager::Render()
{
	//	�X�e�[�g�ݒ�
	Graphics::Instance().GetShader()->SetDepthStencilState(Shader::DEPTH_STENCIL_STATE::ZT_ON_ZW_ON);
	Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ALPHA);
	Graphics::Instance().GetShader()->SetRasterizerState(Shader::RASTERIZER_STATE::CULL_NONE);

	//	�`��
	for (UI*& ui : userInterfaces_)
	{
		ui->Render();
	}
}

void UIManager::DrawDebug()
{
	int size = userInterfaces_.size();

	//	UImanager�f�o�b�O
	if (ImGui::TreeNode("UIManager"))
	{
		ImGui::DragInt("UI Count", &size);
		if(ImGui::Checkbox("AllDrawFlag", &allDrawFlag_))
		{ 
			SetDrawFlag(allDrawFlag_);
		}

		//	UI�f�o�b�O
		for (UI*& ui : userInterfaces_)
		{
			ui->DrawDebug();
		}

		ImGui::TreePop();
	}

}