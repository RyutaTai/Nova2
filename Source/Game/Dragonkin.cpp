#include "Dragonkin.h"

#include "../Nova/Graphics/Graphics.h"

//	コンストラクタ
Dragonkin::Dragonkin()
	:Enemy("./Resources/Model/silver-dragonkin-mir4/source/Silver_Dragonkin/Mon_BlackDragon31_Skeleton2.gltf")
{
	myType_ = EnemyType::Dragonkin;
	
	//	ステートセット(Dragonkin::StateTypeの順と合わせる)
	//stateMachine_.reset(new StateMachine<State<Player>>());
	//stateMachine_->RegisterState(new PlayerState::IdleState(this));		//	待機
	//stateMachine_->RegisterState(new PlayerState::MoveState(this));		//	移動
	//stateMachine_->RegisterState(new PlayerState::AttackState(this));		//	攻撃
	//stateMachine_->RegisterState(new PlayerState::ComboOne1(this));		//	コンボ0_1
	//stateMachine_->RegisterState(new PlayerState::ComboOne2(this));		//	コンボ0_2
	//stateMachine_->RegisterState(new PlayerState::ComboOne3(this));		//	コンボ0_3
	//stateMachine_->RegisterState(new PlayerState::ComboOne4(this));		//	コンボ0_4
	//stateMachine_->RegisterState(new PlayerState::ComboOne5(this));		//	コンボ0_5
	//stateMachine_->RegisterState(new PlayerState::ComboOne6(this));		//	コンボ0_6
	//stateMachine_->RegisterState(new PlayerState::ComboOne7(this));		//	コンボ0_7
	//stateMachine_->RegisterState(new PlayerState::DodgeState(this));		//	回避

	//	モデルのルート設定
	int rootNodeIndex = GetNodeIndex("root");
	SetRootJointIndex(rootNodeIndex);


}

//	初期化
void Dragonkin::Initialize()
{

	//stateMachine_->SetState(static_cast<int>(StateType::Idle));			//	初期ステートセット
	//PlayAnimation(Player::AnimationType::Idle, true, 1.0f, 0.0f);			//	待機アニメーション再生
	//SetAnimation(DragonkinAnimation::ANIM_IDLE02);						//	待機アニメーションセット

	//	位置設定
	GetTransform()->SetPosition({ 23.0f, 0.0f,3.0f });


	//	座標系変換
	//GetTransform()->SetCoordinateSystem(Transform::CoordinateSystem::cRightYup);

	//	スケール
	float scale = 0.04f;
	GetTransform()->SetScaleFactor(scale);

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

//	破棄処理
void Dragonkin::Destroy()
{

	Enemy::Destroy();	//	自身を破棄
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
		Character::DrawDebug();
		ImGui::DragFloat3("moveVec", &moveVec_.x, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat("AnimationSpeed", &animationSpeed_, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::TreePop();
	}
}