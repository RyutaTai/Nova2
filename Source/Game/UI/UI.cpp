#include "UI.h"

#include "../../imgui/imgui.h"
#include "../../Nova/Resources/ResourceManager.h"
#include "UIManager.h"

UI::UI()
{
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
	
}

//	デバッグ描画
void UI::DrawDebug()
{
	
}