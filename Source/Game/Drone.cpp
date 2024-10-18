#include "Drone.h"

#include "../Nova/Graphics/Graphics.h"
#include "../Nova/Input/Input.h"
#include "../Nova/Core/Framework.h"
#include "../Nova/Others/MathHelper.h"
#include "../Nova/Others/Converter.h"
#include "DroneState.h"
#include "BulletStraight.h"
#include "BulletHorming.h"
#include "Player.h"

//	コンストラクタ
Drone::Drone()
	:Enemy("./Resources/Model/Drone/Drone.glb")
{
	//	ステートセット(Drone::StateTypeの順と合わせる)
	stateMachine_.reset(new StateMachine<State<Drone>>());
	stateMachine_->RegisterState(new DroneState::IdleState(this));			//	待機
	stateMachine_->RegisterState(new DroneState::SearchState(this));		//	探索
	stateMachine_->RegisterState(new DroneState::MoveState(this));			//	移動
	stateMachine_->RegisterState(new DroneState::AttackState(this));		//	攻撃
	stateMachine_->RegisterState(new DroneState::AvoidanceState(this));		//	回避

	//	初期ステート設定
	stateMachine_->SetState(static_cast<int>(StateType::Idle));

}

Drone::~Drone()
{
	if (sources_[static_cast<int>(Audio3D::Shot)])
	{
		delete sources_[static_cast<int>(Audio3D::Shot)];
	}
}

//	初期化
void Drone::Initialize()
{
	//	アニメーションセット
	myType_ = ENEMY_TYPE::DRONE;

	//	初期位置設定
	/*DirectX::XMFLOAT3 position = { 190,700,1620 };
	GetTransform()->SetPosition(position);*/

	//	初期角度設定
	float angleY = ConvertToRadian(220.0f);
	GetTransform()->SetRotationY(angleY);

	//	スケール
	float scale = 1.0f;
	//float scale = 10.0f;
	GetTransform()->SetScaleFactor(scale);

	//	半径設定
	height_ = 4.4f;
	radius_ = 2.5f;

	//	索敵範囲設定
	searchRange_ = 15.0f;

	//	HP設定
	hp_ = MAX_HP;

	//	弾丸初期化
	BulletManager::Instance().Initialize();

	//	エフェクト読み込み
	effectResource_ = ResourceManager::Instance().LoadEffectResource("./Resources/Effect/HitEff.efk");

	//	エフェクトスケール設定
	effectScale_ = 80.0f;

	/* ----- オーディオ初期化 ----- */
#if 1
	DirectX::XMFLOAT3 playerPos = Player::Instance().GetTransform()->GetPosition();
	float playerHeight = Player::Instance().GetHeight();
	float posOffsetY = -10.0f;
	emitter_[static_cast<int>(Audio3D::Shot)].position_ = GetTransform()->GetPosition();
	//emitter_[static_cast<int>(Audio3D::Shot)].position.y = playerPos.y + playerHeight / 2.0f + posOffsetY;
	emitter_[static_cast<int>(Audio3D::Shot)].velocity_ = { 1.0f, 2.0f, 1.0f };
	emitter_[static_cast<int>(Audio3D::Shot)].minDistance_ = 7.0f;
	emitter_[static_cast<int>(Audio3D::Shot)].maxDistance_ = 12.0f;
	emitter_[static_cast<int>(Audio3D::Shot)].volume_ = 2.0f;
	sources_[static_cast<int>(Audio3D::Shot)] = Audio::Instance().LoadAudioSource3D("./Resources/Audio/SE/shot.wav", &emitter_[static_cast<int>(Audio3D::Shot)]);
	//sources_[static_cast<int>(Audio3D::Shot)] = Audio::Instance().LoadAudioSource3D("./Resources/Audio/BGM/Title.wav", &emitter_[static_cast<int>(Audio3D::Shot)]);
#endif

	//	発射音再生
#if 0
	if (sources_[static_cast<int>(Audio3D::Shot)])
	{
		sources_[static_cast<int>(Audio3D::Shot)]->Play(true);
	}
#endif


}

//	更新処理
void Drone::Update(const float& elapsedTime)
{
	//	ステート毎の更新処理
	stateMachine_->Update(elapsedTime);

	//	次の弾を発射するまでのタイマー更新
	launchTimer_ -= elapsedTime;

	//	プレイヤーを見つけたら
	if (SearchPlayer())
	{
		//	弾丸処理
		LaunchBullet();

	}

	//	旋回処理
	Turn(elapsedTime);

	//	弾丸があれば
	if (BulletManager::Instance().GetBulletCount() > 0)
	{
		//	弾丸更新処理
		BulletManager::Instance().Update(elapsedTime);
		BulletManager::Instance().CoverModelUpdate(elapsedTime);
	}

	//	HPがなくなったら
	if (hp_ <= 0)
	{
		Destroy();
	}

	//	エミッター更新
	UpdateEmitter();
	UpdateAudioSource(elapsedTime);
	
}

//	エミッター更新
void Drone::UpdateEmitter()
{
	emitter_[static_cast<int>(Audio3D::Shot)].position_ = GetTransform()->GetPosition();
	//emitter_[static_cast<int>(Audio_3d::Shot)].velocity = {1,2,1};

}

//	オーディオソース更新
void Drone::UpdateAudioSource(const float& elapsedTime)
{
	if (sources_[static_cast<int>(Audio3D::Shot)])
	{
		sources_[static_cast<int>(Audio3D::Shot)]->SetDSPSetting(Player::Instance().GetListener());
		sources_[static_cast<int>(Audio3D::Shot)]->Update(elapsedTime);
	}

}

//	弾丸処理
void Drone::LaunchBullet()
{
	if (bulletLaunch_)	//	弾丸発射フラグが立っていたら(デバッグ用)
	{

#if 1
		if (launchTimer_ <= 0.0f)	//	一定間隔で弾を発射
#else
		GamePad gamePad = Input::Instance().GetGamePad();
		if (gamePad.GetButtonDown() & GamePad::BTN_START)	//	Enterキーで発射
#endif
		{
			//	前方向
			DirectX::XMFLOAT3 dir = {};
			float angleY = GetTransform()->GetRotationY();

			dir.x = sinf(angleY);
			dir.y = 0.0f;
			dir.z = cosf(angleY);

			//	発射位置
			DirectX::XMFLOAT3 pos = this->GetTransform()->GetPosition();
			pos = pos + dir * 2.0f;

			//	弾丸ファイル名
			const char* bulletName = "./Resources/Model/Bullet/Sphere.glb";
#if  0	//	直進する弾丸生成
			BulletStraight* bullet = new BulletStraight(bulletName);
			bullet->Launch(dir, pos);

#else	//	追従する弾丸生成
			BulletHorming* bullet = new BulletHorming(bulletName);
			bullet->Launch(dir, pos);
#endif	
			//	所有者の位置設定
			bullet->SetOwnerPosition(this->GetTransform()->GetPosition());

			//	発射タイマーリセット
			launchTimer_ = 3.0f;

			//	発射音再生
			if (sources_[static_cast<int>(Audio3D::Shot)])
			{
				sources_[static_cast<int>(Audio3D::Shot)]->Play(false);
			}

		}
	}
}

//	旋回処理
void Drone::Turn(const float& elapsedTime)
{
	//	プレイヤーをターゲットに設定
	DirectX::XMFLOAT3 playerPos = Player::Instance().GetTransform()->GetPosition();
	SetTargetPosition(playerPos);

	//	ターゲット方向への進行ベクトルを算出(単位ベクトル化はTurn関数内で行っている)
	DirectX::XMFLOAT3 dronePos = this->GetTransform()->GetPosition();
	float vx = targetPosition_.x - dronePos.x;
	float vz = targetPosition_.z - dronePos.z;


	//	ドローン用の回転処理
#if 0
	DirectX::XMVECTOR TargetPos = DirectX::XMLoadFloat3(&targetPosition_);				//	ターゲットの位置ベクトル
	DirectX::XMVECTOR DronePos = DirectX::XMLoadFloat3(&GetTransform()->GetPosition());	//	ドローンの位置ベクトル
	DirectX::XMVECTOR DroneToTarget = DirectX::XMVectorSubtract(TargetPos, DronePos);	//	ドローンからターゲットへのベクトル
	DroneToTarget = DirectX::XMVector3Normalize(DroneToTarget);							//	ターゲットへのベクトルを正規化	
	DirectX::XMVECTOR Front = DirectX::XMLoadFloat3(&GetTransform()->CalcForward());	//	ドローンの前方向
	Front = DirectX::XMVector3Normalize(Front);											//	ドローンの前方向ベクトルを正規化
	
	DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Front, DroneToTarget);				//	外積を行い、回転軸を算出
	float rad = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Front, DroneToTarget));		//	内積する
	rad = acosf(rad);	//	内積の結果から回転角度を求める

	if (fabsf(rad) > 1e-8f)
	{
		//DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationAxis(Axis, rad * elapsedTime);											//	回転軸Axisと回転角度radから回転クオータニオンを求める
		//DirectX::XMVECTOR Rotate = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&GetTransform()->GetRotation()), Q);		//	求めたクオータニオンをかけ合わせる
		//DirectX::XMFLOAT4 rotation;
		//DirectX::XMStoreFloat4(&rotation, Rotate);
		//GetTransform()->SetRotation(rotation);

		DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationAxis(Axis, rad);														//	回転軸Axisと回転角度radから回転クオータニオンを求める
		DirectX::XMVECTOR Rotate = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&GetTransform()->GetRotation()), Q);		//	求めたクオータニオンをかけ合わせる
		DirectX::XMFLOAT4 rotation;
		DirectX::XMStoreFloat4(&rotation, Rotate);
		GetTransform()->SetRotation(rotation);
	}

#endif

	//	旋回処理
#if 1
	Character::Turn(elapsedTime, vx, vz, turnSpeed_);
#endif
}

//	ステージとの当たり判定
bool Drone::RayVsVertical(const float& elapsedTime)
{

	return false;
}

bool Drone::RayVsHorizontal(const float& elapsedTime)
{

	return false;
}

//	破棄処理
void Drone::Destroy()
{
	//	エフェクト再生
	DirectX::XMFLOAT3 effectPos = GetTransform()->GetPosition();
	effectResource_->Play(effectPos, effectScale_);
	
	Enemy::Destroy();	//	自身を破棄
}

//	描画処理
void Drone::Render()
{
	//	ドローン描画
	//	ピクセルシェーダーセット
	//SetPixelShader("./Resources/Shader/DronePS.cso");
	Character::Render();

	//	弾丸描画
	BulletManager::Instance().Render();

}

//	デバッグプリミティブ描画
void Drone::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//	衝突判定用のデバッグ球を描画
	debugRenderer->DrawCylinder(this->GetTransform()->GetPosition(), radius_, height_, DirectX::XMFLOAT4(0, 0, 0, 1));

	//	索敵範囲描画(円柱)
	debugRenderer->DrawCylinder(this->GetTransform()->GetPosition(), searchRange_, 1.0f, { 0,1,0.1f,1.0f });
	
	//	弾丸のデバッグ球描画
	BulletManager::Instance().DrawDebugPrimitive();

}

//	デバッグ描画
void Drone::DrawDebug()
{
	float scale = GetTransform()->GetScaleFactor();

	if (ImGui::TreeNode(u8"Drone ドローン"))
	{
		GetTransform()->DrawDebug();
		Character::DrawDebug();
		ImGui::DragFloat("ScaleFactor", &scale,1.0f, -FLT_MAX, FLT_MAX);	//	スケール
		ImGui::DragFloat("TurnSpeed", &turnSpeed_, 1.0f, -FLT_MAX, FLT_MAX);	//	旋回速度
		ImGui::DragFloat("SerchRange", &searchRange_, 0.1f, -FLT_MAX, FLT_MAX);
		ImGui::Checkbox("Bullet Launch ", &bulletLaunch_);						//	弾丸発射
		ImGui::TreePop();
	}
	BulletManager::Instance().DrawDebug();	//	弾丸ImGui
	
	GetTransform()->SetScaleFactor(scale);

}