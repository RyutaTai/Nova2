#include "Dragonkin.h"

#include "../Nova/Graphics/Graphics.h"

//	コンストラクタ
Dragonkin::Dragonkin()
	:Enemy("./Resources/Model/silver-dragonkin-mir4/source/Silver_Dragonkin/Mon_BlackDragon31_Skeleton2")
{
	
}

//	初期化
void Dragonkin::Initialize()
{
	myType_ = ENEMY_TYPE::DRAGONKIN;
	//SetAnimation(DragonkinAnimation::ANIM_IDLE02);	//	待機アニメーションセット
}

//	更新処理
void Dragonkin::Update(const float& elapsedTime)
{

}

//	ステージとの当たり判定
bool Dragonkin::RayVsVertical(const float& elapsedTime)
{

	return false;
}

bool Dragonkin::RayVsHorizontal(const float& elapsedTime)
{

	return false;
}

//	描画処理
void Dragonkin::Render()
{
	//	ピクセルシェーダーセット
	//Graphics::Instance().GetShader()->CreatePsFromCso(Graphics::Instance().GetDevice(), "./Resources/Shader/DronePS.cso", pixelShader_.ReleaseAndGetAddressOf());
	//this->SetPixelShader(pixelShader_.Get());

	Character::Render();
}

//	デバッグプリミティブ描画
void Dragonkin::DrawDebugPrimitive()
{
	//DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
	//
	////	衝突判定用のデバッグ球を描画
	//debugRenderer->DrawSphere(this->GetTransform()->GetPosition(), radius_, DirectX::XMFLOAT4(0, 0, 0, 1));
	//debugRenderer->DrawSphere(this->GetTransform()->GetPosition(), radius_, DirectX::XMFLOAT4(0, 0, 0, 1));
	//
	////	索敵範囲描画(円柱)
	//debugRenderer->DrawCylinder(this->GetTransform()->GetPosition(), searchRange_, 30.0f, { 0,1,0.1f,1.0f });

}

//	デバッグ描画
void Dragonkin::DrawDebug()
{
	if (ImGui::TreeNode(u8"Dragonkin竜人"))
	{
		//GetTransform()->DrawDebug();
		ImGui::DragFloat3("moveVec", &moveVec_.x, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat("AnimationSpeed", &animationSpeed_, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::TreePop();
	}
}