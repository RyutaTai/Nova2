#include "Dragonkin.h"

#include "DragonkinAction.h"
#include "DragonkinJudgment.h"
#include "Player.h"
#include "../Nova/AI/BehaviorData.h"
#include "../Nova/Graphics/Graphics.h"
#include "../Nova/Collision/Collision.h"

//	コンストラクタ
Dragonkin::Dragonkin()
	:Enemy("./Resources/Model/silver-dragonkin-mir4/source/Silver_Dragonkin/Mon_BlackDragon31_Skeleton2.gltf")
{
	//	自身の種類設定
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

	//	----- Collision -----
	RegisterCollisionData();

	//	索敵範囲設定
	searchRange_ = 13.5f;

	//	当たり判定用高さ、半径設定
	//radius_ = 1.79f;
	radius_ = 3.0f;
	height_ = 10.0f;
	useOffsetY_ = false;

	//	HP設定
	hp_ = MaxHp_;

	//	ビヘイビアツリー設定
	behaviorData_ = new BehaviorData();
	behaviorTree_ = new BehaviorTree(this);

	behaviorTree_->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority, nullptr, nullptr);										//	ルートノード
	behaviorTree_->AddNode("Root", "Idle", 0, BehaviorTree::SelectRule::Non, new DragonkinJudgment::IdleJudgment(this), new DragonkinAction::IdleAction(this));			//	待機ノード(末端)
	behaviorTree_->AddNode("Root", "Battle", 1, BehaviorTree::SelectRule::Random, new DragonkinJudgment::BattleJudgment(this), nullptr);										//	戦闘ノード(中間)

	behaviorTree_->AddNode("Battle", "AttackPunch", 0, BehaviorTree::SelectRule::Non, nullptr, new DragonkinAction::AttackPunchAction(this));	//	通常パンチ攻撃(末端)
	behaviorTree_->AddNode("Battle", "AttackKick", 1, BehaviorTree::SelectRule::Non, nullptr, new DragonkinAction::AttackKickAction(this));	//	通常キック攻撃(末端)
	behaviorTree_->AddNode("Battle", "AttackWing", 2, BehaviorTree::SelectRule::Non, nullptr, new DragonkinAction::AttackWingAction(this));	//	通常キック攻撃(末端)

}

//	初期化
void Dragonkin::Initialize()
{
	//stateMachine_->SetState(static_cast<int>(StateType::Idle));			//	初期ステートセット
	//PlayAnimation(Player::AnimationType::Idle, true, 1.0f, 0.0f);			//	待機アニメーション再生
	//SetAnimation(DragonkinAnimation::Idle02);						//	待機アニメーションセット

	//	位置設定
	GetTransform()->SetPosition({ 23.0f, 0.0f,3.0f });

	//	回転値設定
	GetTransform()->SetRotationY(DirectX::XMConvertToRadians(-182.499f));

	//	座標系変換
	//GetTransform()->SetCoordinateSystem(Transform::CoordinateSystem::cRightYup);

	//	スケール
	//GetTransform()->SetScaleFactor(0.04f);
	//GetTransform()->SetScaleFactor(0.02f);
	GetTransform()->SetScaleFactor(0.015f);

	//	初期アニメーション再生速度設定
	SetAnimationSpeed(1.0f);
}

//	当たり判定登録
void Dragonkin::RegisterCollisionData()
{
#pragma region ----- 押し出し判定登録 -----
	//	{名前、半径、  Y軸を固定するか、オフセット位置、更新名、	デフォルトカラー、	ヒットカラー}
	//	{name, radius, fixedY,			offsetPosition,	updateName,	defaultColor,		hitColor}

	RegisterCollisionDetectionData({ "Head",		0.4f,false,{},"head" });		//	頭
	RegisterCollisionDetectionData({ "Chest",		0.4f,false,{},"spine_02" });		//	胸部
	RegisterCollisionDetectionData({ "Shoulder_L",	0.4f,false,{},"Shoulder_L" });		//	左肩
	RegisterCollisionDetectionData({ "Shoulder_R",	0.4f,false,{},"Shoulder_R" });		//	右肩
	RegisterCollisionDetectionData({ "lowerarm_l",	0.4f,false,{},"lowerarm_l" });		//	左肘
	RegisterCollisionDetectionData({ "lowerarm_r",	0.4f,false,{},"lowerarm_r" });		//	右肘
	RegisterCollisionDetectionData({ "Hand_L",		0.4f,false,{},"Hand_L" });		//	左手首
	RegisterCollisionDetectionData({ "Hand_R",		0.4f,false,{},"Hand_R" });		//	右手首
	RegisterCollisionDetectionData({ "spine_01",	0.6f,false,{},"spine_01" });		//	腰
	RegisterCollisionDetectionData({ "calf_l",		0.4f,false,{},"calf_l" });		//	左膝
	RegisterCollisionDetectionData({ "calf_r",		0.4f,false,{},"calf_r" });		//	右膝
	RegisterCollisionDetectionData({ "Foot_L",		0.6f,false,{},"Foot_L" });		//	左足首
	RegisterCollisionDetectionData({ "Foot_R",		0.6f,false,{},"Foot_R" });		//	右足首

	//	左の翼
	RegisterCollisionDetectionData({ "Wing_L03",	0.4f,false,{},"Wing_L03" });		//	一番付け根に近い
	RegisterCollisionDetectionData({ "Wing_L04",	0.4f,false,{},"Wing_L04" });
	RegisterCollisionDetectionData({ "Wing_L05",	0.4f,false,{},"Wing_L05" });
	RegisterCollisionDetectionData({ "Wing_L06",	0.4f,false,{},"Wing_L06" });
	RegisterCollisionDetectionData({ "Wing_L08",	0.4f,false,{},"Wing_L08" });
	RegisterCollisionDetectionData({ "Wing_L09",	0.4f,false,{},"Wing_L09" });
	RegisterCollisionDetectionData({ "Wing_L10",	0.4f,false,{},"Wing_L10" });		//	一番先の方

	//	右の翼
	RegisterCollisionDetectionData({ "Wing_R03",	0.4f,false,{},"Wing_R03" });		//	一番付け根に近い
	RegisterCollisionDetectionData({ "Wing_R04",	0.4f,false,{},"Wing_R04" });
	RegisterCollisionDetectionData({ "Wing_R05",	0.4f,false,{},"Wing_R05" });
	RegisterCollisionDetectionData({ "Wing_R06",	0.4f,false,{},"Wing_R06" });
	RegisterCollisionDetectionData({ "Wing_R08",	0.4f,false,{},"Wing_R08" });
	RegisterCollisionDetectionData({ "Wing_R09",	0.4f,false,{},"Wing_R09" });
	RegisterCollisionDetectionData({ "Wing_R10",	0.4f,false,{},"Wing_R10" });		//	一番先の方

#pragma endregion ----- 押し出し判定登録 -----

#pragma region ----- くらい判定登録 -----
	//	{名前、半径、	オフセット位置、ダメージ倍率、	更新名、	デフォルトカラー、	ヒットカラー}
	//	{name, radius,	offsetPos,		damage,			updateName,	defaultColor,		hitColor}

	RegisterDamageDetectionData({ "head",			0.4f,{},1.0f, "head" });		//	頭
	RegisterDamageDetectionData({ "spine_02",		0.4f,{},1.0f, "spine_02" });		//	胸部
	RegisterDamageDetectionData({ "Shoulder_L",		0.4f,{},1.0f, "Shoulder_L" });		//	左肩
	RegisterDamageDetectionData({ "Shoulder_R",		0.4f,{},1.0f, "Shoulder_R" });		//	右肩
	RegisterDamageDetectionData({ "lowerarm_l",		0.4f,{},1.0f, "lowerarm_l" });		//	左肘
	RegisterDamageDetectionData({ "lowerarm_r",		0.4f,{},1.0f, "lowerarm_r" });		//	右肘
	RegisterDamageDetectionData({ "Hand_L",			0.4f,{},1.0f, "Hand_L" });		//	左手首
	RegisterDamageDetectionData({ "Hand_R",			0.4f,{},1.0f, "Hand_R" });		//	右手首
	RegisterDamageDetectionData({ "spine_01",		0.4f,{},1.0f, "spine_01" });		//	腰
	RegisterDamageDetectionData({ "calf_l",			0.4f,{},1.0f, "calf_l" });		//	左膝
	RegisterDamageDetectionData({ "calf_r",			0.4f,{},1.0f, "calf_r" });		//	右膝
	RegisterDamageDetectionData({ "Foot_L",			0.6f,{},1.0f, "Foot_L" });		//	左足首
	RegisterDamageDetectionData({ "Foot_R",			0.6f,{},1.0f, "Foot_R" });		//	右足首

	//	左の翼
	RegisterDamageDetectionData({ "Wing_L03",		0.4f,{},1.0f, "Wing_L03" });		//	一番付け根に近い
	RegisterDamageDetectionData({ "Wing_L04",		0.4f,{},1.0f, "Wing_L04" });
	RegisterDamageDetectionData({ "Wing_L05",		0.4f,{},1.0f, "Wing_L05" });
	RegisterDamageDetectionData({ "Wing_L06",		0.4f,{},1.0f, "Wing_L06" });
	RegisterDamageDetectionData({ "Wing_L08",		0.4f,{},1.0f, "Wing_L08" });
	RegisterDamageDetectionData({ "Wing_L09",		0.4f,{},1.0f, "Wing_L09" });
	RegisterDamageDetectionData({ "Wing_L10",		0.4f,{},1.0f, "Wing_L10" });		//	一番先の方

	//	右の翼
	RegisterDamageDetectionData({ "Wing_R03",		0.4f,{},1.0f, "Wing_R03" });		//	一番付け根に近い
	RegisterDamageDetectionData({ "Wing_R04",		0.4f,{},1.0f, "Wing_R04" });
	RegisterDamageDetectionData({ "Wing_R05",		0.4f,{},1.0f, "Wing_R05" });
	RegisterDamageDetectionData({ "Wing_R06",		0.4f,{},1.0f, "Wing_R06" });
	RegisterDamageDetectionData({ "Wing_R08",		0.4f,{},1.0f, "Wing_R08" });
	RegisterDamageDetectionData({ "Wing_R09",		0.4f,{},1.0f, "Wing_R09" });
	RegisterDamageDetectionData({ "Wing_R10",		0.4f,{},1.0f, "Wing_R10" });		//	一番先の方

#pragma endregion ----- くらい判定登録 -----

#pragma region ----- 攻撃判定登録 -----
	//	{名前、半径、	オフセット位置、更新名、	デフォルトカラー、	ヒットカラー}
	//	{name, radius,	offsetPos,		updateName, defaultColor,		hitColor}

	RegisterAttackDetectionData({ "head",			0.4f,{}, "head" });		//	頭
	RegisterAttackDetectionData({ "spine_02",		0.4f,{}, "spine_02" });		//	胸部
	RegisterAttackDetectionData({ "Shoulder_L",		0.4f,{}, "Shoulder_L" });		//	左肩
	RegisterAttackDetectionData({ "Shoulder_R",		0.4f,{}, "Shoulder_R" });		//	右肩
	RegisterAttackDetectionData({ "lowerarm_l",		0.4f,{}, "lowerarm_l" });		//	左肘
	RegisterAttackDetectionData({ "lowerarm_r",		0.4f,{}, "lowerarm_r" });		//	右肘
	RegisterAttackDetectionData({ "Hand_L",			0.4f,{}, "Hand_L" });		//	左手首
	RegisterAttackDetectionData({ "Hand_R",			0.4f,{}, "Hand_R" });		//	右手首
	RegisterAttackDetectionData({ "spine_01",		0.4f,{}, "spine_01" });		//	腰
	RegisterAttackDetectionData({ "calf_l",			0.4f,{}, "calf_l" });		//	左膝
	RegisterAttackDetectionData({ "calf_r",			0.4f,{}, "calf_r" });		//	右膝
	RegisterAttackDetectionData({ "Foot_L",			0.6f,{}, "Foot_L" });		//	左足首
	RegisterAttackDetectionData({ "Foot_R",			0.4f,{}, "Foot_R" });		//	右足首

	//	左の翼
	RegisterAttackDetectionData({ "Wing_L03",		0.4f,{}, "Wing_L03" });		//	一番付け根に近い
	RegisterAttackDetectionData({ "Wing_L04",		0.4f,{}, "Wing_L04" });
	RegisterAttackDetectionData({ "Wing_L05",		0.4f,{}, "Wing_L05" });
	RegisterAttackDetectionData({ "Wing_L06",		0.4f,{}, "Wing_L06" });
	RegisterAttackDetectionData({ "Wing_L08",		0.4f,{}, "Wing_L08" });
	RegisterAttackDetectionData({ "Wing_L09",		0.4f,{}, "Wing_L09" });
	RegisterAttackDetectionData({ "Wing_L10",		0.4f,{}, "Wing_L10" });		//	一番先の方

	//	右の翼												  
	RegisterAttackDetectionData({ "Wing_R03",		0.4f,{}, "Wing_R03" });		//	一番付け根に近い
	RegisterAttackDetectionData({ "Wing_R04",		0.4f,{}, "Wing_R04" });
	RegisterAttackDetectionData({ "Wing_R05",		0.4f,{}, "Wing_R05" });
	RegisterAttackDetectionData({ "Wing_R06",		0.4f,{}, "Wing_R06" });
	RegisterAttackDetectionData({ "Wing_R08",		0.4f,{}, "Wing_R08" });
	RegisterAttackDetectionData({ "Wing_R09",		0.4f,{}, "Wing_R09" });
	RegisterAttackDetectionData({ "Wing_R10",		0.4f,{}, "Wing_R10" });		//	一番先の方

#pragma endregion ----- 攻撃判定登録 -----
}

//	更新処理
void Dragonkin::Update(const float& elapsedTime)
{
	//	更新フラグがfalseなら処理しない
	if (updateFlag_ == false)return;

	Character::Update(elapsedTime);

	//	アニメーション更新処理
	UpdateAnimation(elapsedTime);

	//	ビヘイビアツリー更新
	UpdateBehaviorTree(elapsedTime);

	//	Collision更新
	UpdateCollisions(elapsedTime);

	//	HPがなくなったら
	if (hp_ <= 0)
	{
		Destroy();
	}

}

//	ビヘイビアツリー更新処理
void Dragonkin::UpdateBehaviorTree(const float& elapsedTime)
{
	//	ビヘイビアツリー更新フラグがfalseなら更新しない
	if (behaviorTreeUpdateFlag_ == false)return;

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
void Dragonkin::PlayAnimation(const AnimationType& animType, const bool& loop, const float& blendTime, const float& animSpeed, const float& startFrame, const float& endFrame)
{
	Character::PlayAnimation(static_cast<int>(animType), loop, blendTime, animSpeed, startFrame, endFrame);
}

//	当たり判定更新
void Dragonkin::UpdateCollisions(const float& elapsedTime)
{
	//	くらい判定更新
	for (DamageDetectionData& data : damageDetectionData_)
	{
		//	ジョイントの名前で位置設定(名前がジョイントの名前ではないとき別途更新必要)
		data.SetJointPosition(GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition()));

		data.Update(elapsedTime);
	}

	//	攻撃判定更新
	for (AttackDetectionData& data : attackDetectionData_)
	{
		//	ジョイントの名前で位置設定(名前がジョイントの名前ではないとき別途更新必要)
		data.SetJointPosition(GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition()));
	}

	/*for (int i = AttackData::TrunAttackStart; i <= AttackData::TackleAttackEnd; ++i)
	{
		AttackDetectionData& data = GetAttackDetectionData(i);
		DirectX::XMFLOAT3 pos = data.GetPosition();
		pos.y = 1.0f;
		data.SetJointPosition(pos);
	}*/

	//	押し出し判定更新
	for (CollisionDetectionData& data : collisionDetectionData_)
	{
		//	ジョイントの名前で位置設定(名前がジョイントの名前ではないとき別途更新必要)
		DirectX::XMFLOAT3 pos = GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition());

		//	Y軸固定
		if (data.GetFixedY())
			pos.y = 0.0f;

		data.SetPosition(pos);
		//data.SetJointPosition(pos);
	}

}

//	破棄処理
void Dragonkin::Destroy()
{
	//	自身を破棄
	Enemy::Destroy();
}

//	描画処理
void Dragonkin::Render()
{
	//	ピクセルシェーダーセット
	//Graphics::Instance().GetShader()->CreatePsFromCso(Graphics::Instance().GetDevice(), "./Resources/Shader/DronePS.cso", pixelShader_.ReleaseAndGetAddressOf());
	//this->SetPixelShader(pixelShader_.Get());

	//	描画
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

	//	----- Collision -----
	if (isCollisionSphere_)
	{
		for (auto& data : GetCollisionDetectionData())
		{
			// 現在アクティブではないので表示しない
			if (data.GetIsActive() == false) continue;

			debugRenderer->DrawSphere(data.GetPosition(), data.GetRadius(), data.GetColor());
		}
	}
	if (isDamageSphere_)
	{
		for (auto& data : GetDamageDetectionData())
		{
			debugRenderer->DrawSphere(data.GetPosition(), data.GetRadius(), data.GetColor());
		}
	}
	if (isAttackSphere_)
	{
		for (auto& data : GetAttackDetectionData())
		{
			// 現在アクティブではないでの表示しない
			if (data.GetIsActive() == false) continue;

			debugRenderer->DrawSphere(data.GetPosition(), data.GetRadius(), data.GetColor());
		}
	}
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
		//	----- 更新フラグ -----
		ImGui::Checkbox("UpdateFlag", &updateFlag_);	//	更新フラグ

		//	----- ビヘイビアツリー -----
		ImGui::Text(u8"Behavior　%s", str.c_str());								//	現在のビヘイビア
		ImGui::Checkbox("BehaviorTreeUpdateFlag", &behaviorTreeUpdateFlag_);	//	ビヘイビアツリー更新フラグ

		//	----- コリジョン描画フラグ -----
		ImGui::Checkbox("IsCollisionSphere", &isCollisionSphere_);	//	押し出し判定
		ImGui::Checkbox("IsAttackSphere", &isAttackSphere_);		//	攻撃判定
		ImGui::Checkbox("IsDamageSphere", &isDamageSphere_);		//	くらい判定

		Character::DrawDebug();

		ImGui::DragFloat3("moveVec", &moveVec_.x, 0.01f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat("SearchRange", &searchRange_, 0.01f);


		ImGui::TreePop();
	}
}