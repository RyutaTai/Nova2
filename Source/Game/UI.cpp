#include "UI.h"

#include "../../imgui/imgui.h"
#include "../Nova/Resources/ResourceManager.h"
#include "UIManager.h"

UI::UI(const wchar_t* filename)
{
	//	スプライト読み込み
	sprite_ = std::make_unique<Sprite>(filename);
		//ResourceManager::Instance().LoadSpriteResource(filename);

	//	生成時にマネージャーに登録
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
		if (sprite_ != nullptr)sprite_->Render();
	}
}

//	デバッグ描画
void UI::DrawDebug()
{
	if (name_ == "")
	{
		name_ = "UI Name";
	}

	if (ImGui::TreeNode(name_.c_str()))
	{
		sprite_->GetTransform()->DrawDebug();
		ImGui::TreePop();
	}
}