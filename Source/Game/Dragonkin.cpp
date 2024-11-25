#include "Dragonkin.h"

#include "DragonkinAction.h"
#include "DragonkinJudgment.h"
#include "../Nova/AI/BehaviorData.h"
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

	//	索敵範囲設定
	searchRange_ = 13.5f;

	//	当たり判定用高さ、半径設定
	//radius_ = 1.79f;
	radius_ = 3.0f;
	height_ = 10.0f;
	useOffsetY_ = false;

	//	HP設定
	hp_ = MAX_HP;

	//	ビヘイビアツリー設定
	behaviorData_ = new BehaviorData();
	behaviorTree_ = new BehaviorTree(this);

	behaviorTree_->AddNode("",			"Root",			0,	BehaviorTree::SelectRule::Priority,	nullptr,											nullptr);										//	ルートノード
	behaviorTree_->AddNode("Root",		"Idle",			0,	BehaviorTree::SelectRule::Non,		new DragonkinJudgment::IdleJudgment(this),			new DragonkinAction::IdleAction(this));			//	待機ノード(末端)
	behaviorTree_->AddNode("Root",		"Battle",		1,	BehaviorTree::SelectRule::Random,	new DragonkinJudgment::BattleJudgment(this),		nullptr);										//	戦闘ノード(中間)

	behaviorTree_->AddNode("Battle",	"AttackPunch",	0,	BehaviorTree::SelectRule::Non,		nullptr,											new DragonkinAction::AttackPunchAction(this));	//	通常パンチ攻撃(末端)
	behaviorTree_->AddNode("Battle",	"AttackKick",	1,	BehaviorTree::SelectRule::Non,		nullptr,											new DragonkinAction::AttackKickAction(this));	//	通常キック攻撃(末端)
	behaviorTree_->AddNode("Battle",	"AttackWing",	2,	BehaviorTree::SelectRule::Non,		nullptr,											new DragonkinAction::AttackWingAction(this));	//	通常キック攻撃(末端)

}

//	初期化
void Dragonkin::Initialize()
{
	//stateMachine_->SetState(static_cast<int>(StateType::Idle));			//	初期ステートセット
	//PlayAnimation(Player::AnimationType::Idle, true, 1.0f, 0.0f);			//	待機アニメーション再生
	//SetAnimation(DragonkinAnimation::ANIM_IDLE02);						//	待機アニメーションセット

	//	位置設定
	GetTransform()->SetPosition({ 23.0f, 0.0f,3.0f });

	//	回転値設定
	GetTransform()->SetRotationY(DirectX::XMConvertToRadians(-182.499f));

	//	座標系変換
	//GetTransform()->SetCoordinateSystem(Transform::CoordinateSystem::cRightYup);

	//	スケール
	//GetTransform()->SetScaleFactor(0.04f);
	GetTransform()->SetScaleFactor(0.020f);

	//	初期アニメーション再生速度設定
	SetAnimationSpeed(1.0f);
}

//	更新処理
void Dragonkin::Update(const float& elapsedTime)
{
	//	アニメーション更新処理
	UpdateAnimation(elapsedTime);

	UpdateBehaviorTree(elapsedTime);	//	ビヘイビアツリー更新

	//	HPがなくなったら
	if (hp_ <= 0)
	{
		Destroy();
	}

}

//	ビヘイビアツリー更新処理
void Dragonkin::UpdateBehaviorTree(const float& elapsedTime)
{
	//	現在実行されているノードが無ければ
	if (activeNode_ == nullptr)
	{
		//	次に実行するノードを推論する
		activeNode_ = behaviorTree_->ActiveNodeInference(behaviorData_);
	}
	//	現在実行するノードがあれば
	if (activeNode_ != nullptr)
	{
		//	ビヘイビアツリーからノードを実行
		activeNode_ = behaviorTree_->Run(activeNode_, behaviorData_, elapsedTime);
	}
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

//	アニメーション
void Dragonkin::PlayAnimation(const AnimationType& animType, const bool& loop, const float& blendTime, const float& startFrame, const float& animSpeed)
{
	Character::PlayAnimation(static_cast<int>(animType), loop, blendTime, startFrame, animSpeed);
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
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//	衝突判定用のデバッグ球を描画
	debugRenderer->DrawCylinder(this->GetTransform()->GetPosition(), radius_, height_, DirectX::XMFLOAT4(0, 0, 0, 1));

	//	索敵範囲描画(円柱)
	debugRenderer->DrawCylinder(this->GetTransform()->GetPosition(), searchRange_, 1.0f, { 0,1,0.1f,1.0f });


}

//	デバッグ描画
void Dragonkin::DrawDebug()
{
	std::string str = "";
	if (activeNode_ != nullptr)
	{
		str = activeNode_->GetName();
	}

	if (ImGui::TreeNode(u8"Dragonkin竜人"))
	{
		ImGui::Text(u8"Behavior　%s", str.c_str());	//	現在のビヘイビア
		Character::DrawDebug();
		ImGui::DragFloat3("moveVec", &moveVec_.x, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat("SearchRange", &searchRange_, 0.01f);
		ImGui::TreePop();
	}
}