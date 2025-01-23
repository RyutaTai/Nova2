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
	:Enemy("./Resources/Model/Drone/Drone.gltf")
{
	//	自分の種類を設定
	myType_ = EnemyType::Drone;

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

//	デストラクタ
Drone::~Drone()
{
	for (int index = 0; index < static_cast<int>(Audio3D::Max); ++index)
	{
		delete sources_[index];
	}
}

//	初期化
void Drone::Initialize()
{
	//	アニメーションセット
	myType_ = EnemyType::Drone;

	//	初期位置設定
	/*DirectX::XMFLOAT3 position = { 190,700,1620 };
	GetTransform()->SetPosition(position);*/

	//	初期角度設定
	float angleY = ConvertToRadian(220.0f);
	GetTransform()->SetRotationY(angleY);

	//	スケール
	float scale = 0.6f;
	//float scale = 10.0f;
	GetTransform()->SetScaleFactor(scale);

	//	----- Collision -----
	RegisterCollisionData();

	//	半径設定
	height_ = 4.4f;
	radius_ = 2.5f;

	//	索敵範囲設定
	searchRange_ = 11.0f;

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

	//	エミッターの設定
	emitter_.position_ = GetTransform()->GetPosition();
	//emitter_[static_cast<int>(Audio3D::Shot)].position.y = playerPos.y + playerHeight / 2.0f + posOffsetY;
	emitter_.velocity_ = { 1.0f, 2.0f, 1.0f };
	emitter_.minDistance_ = 7.0f;
	emitter_.maxDistance_ = 12.0f;
	emitter_.volume_ = 1.0f;
	
	//	発射音
	sources_[static_cast<int>(Audio3D::Shot)] = AudioManager::Instance().LoadAudioSource3D("./Resources/Audio/SE/Drone/launchSE.wav", Audio::AudioType::SE3D, "GameScene", &emitter_);
	sources_[static_cast<int>(Audio3D::Shot)]->SetVolume(0.3f, false);
	sources_[static_cast<int>(Audio3D::Shot)]->SetAudioName("LaunchBullet");
	sources_[static_cast<int>(Audio3D::Shot)]->SetDSPSetting(Player::Instance().GetListener());
	AudioManager::Instance().Register(sources_[static_cast<int>(Audio3D::Shot)]);
	
#if 1	//	3dで生成
	/*sources_[static_cast<int>(Audio3D::Move)] = AudioManager::Instance().LoadAudioSource3D("./Resources/Audio/SE/bulletMove.wav", Audio::AudioType::SE3D, "GameScene", &emitter_);
	sources_[static_cast<int>(Audio3D::Move)]->SetVolume(0.3f, false);
	sources_[static_cast<int>(Audio3D::Move)]->SetAudioName("BulletMove");
	AudioManager::Instance().Register(sources_[static_cast<int>(Audio3D::Move)]);*/
#else
	debugSource_ = AudioManager::Instance().LoadAudioSource("./Resources/Audio/SE/bulletMove.wav", Audio::AudioType::SE3D, "GameScene");
	debugSource_->SetVolume(0.3f, false);
	debugSource_->SetAudioName("BulletMove");
	AudioManager::Instance().Register(debugSource_);
#endif
	//	破壊音
	sources_[static_cast<int>(Audio3D::Destroy)] = AudioManager::Instance().LoadAudioSource3D("./Resources/Audio/SE/Bullet/bulletMove.wav", Audio::AudioType::SE3D, "GameScene", &emitter_);
	sources_[static_cast<int>(Audio3D::Destroy)]->SetVolume(0.3f, false);
	sources_[static_cast<int>(Audio3D::Destroy)]->SetAudioName("BulletDestroy");
	AudioManager::Instance().Register(sources_[static_cast<int>(Audio3D::Destroy)]);

#endif

	//	テスト用
#if 0
	sources_[static_cast<int>(Audio3D::Bgm)] = AudioManager::Instance().LoadAudioSource3D("./Resources/Audio/BGM/452_BPM140_2.wav", Audio::AudioType::BGM3D, "GameScene", &emitter_);
	//sources_[static_cast<int>(Audio3D::Bgm)] = AudioManager::Instance().LoadAudioSource3D("./Resources/Audio/BGM/Title.wav", Audio::AudioType::BGM3D, "GameScene", &emitter_);
	sources_[static_cast<int>(Audio3D::Bgm)]->SetVolume(0.2f, false);
	sources_[static_cast<int>(Audio3D::Bgm)]->SetAudioName("TestBGM");
	sources_[static_cast<int>(Audio3D::Bgm)]->SetDSPSetting(Player::Instance().GetListener());
	sources_[static_cast<int>(Audio3D::Bgm)]->SetPlayable(true);	//	再生するかのフラグ
	sources_[static_cast<int>(Audio3D::Bgm)]->Play(true);
	AudioManager::Instance().Register(sources_[static_cast<int>(Audio3D::Bgm)]);
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
	//	----- ステート更新処理 -----
	stateMachine_->Update(elapsedTime);

	//	----- 当たり判定更新 -----
	UpdateCollisions(elapsedTime);

	//	次の弾を発射するまでのタイマー更新
	launchTimer_ -= elapsedTime;

	//	プレイヤーを見つけたら
	if (SearchPlayer())
	{
		//	弾丸処理
		LaunchBullet();

	}

	//	----- 旋回処理 -----
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

	//	----- オーディオ更新 -----
	UpdateEmitter();
	UpdateAudioSource();
	
}

//	エミッター更新
void Drone::UpdateEmitter()
{
	emitter_.position_ = GetTransform()->GetPosition();
	//emitter_[static_cast<int>(Audio_3d::Shot)].velocity = {1,2,1};

}

//	オーディオソース更新
void Drone::UpdateAudioSource()
{
	//	発射音
	if (sources_[static_cast<int>(Audio3D::Shot)])
	{
		sources_[static_cast<int>(Audio3D::Shot)]->SetDSPSetting(Player::Instance().GetListener());
	}
	//	BGM(デバッグ用)
	if (sources_[static_cast<int>(Audio3D::Bgm)])
	{
		sources_[static_cast<int>(Audio3D::Bgm)]->SetDSPSetting(Player::Instance().GetListener());
	}
}

//	弾丸処理
void Drone::LaunchBullet()
{
	//	弾丸発射フラグが立っていたら(デバッグ用)
	if (bulletLaunch_)	
	{

#if 1
		//	一定間隔で弾を発射
		if (launchTimer_ <= 0.0f)
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
			const char* bulletName = "./Resources/Model/Bullet/Sphere.gltf";
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
			launchTimer_ = 3.5f;

			//	発射音再生
#if 0
			if (sources_[static_cast<int>(Audio3D::Shot)])
			{
				sources_[static_cast<int>(Audio3D::Shot)]->Play(false);
			}
#else
			AudioManager::Instance().GetAudioResource("LaunchBullet")->Play(false);
#endif

		}
	}
}

//	旋回処理
void Drone::Turn(const float& elapsedTime)
{
	//	旋回処理しないならreturn
	if (turnAction_ == false)return;

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

//	当たり判定登録
void Drone::RegisterCollisionData()
{
#pragma region ----- 押し出し判定登録 -----
	//	{名前、半径、  Y軸を固定するか、オフセット位置、更新名、	デフォルトカラー、	ヒットカラー}
	//	{name, radius, fixedY,			offsetPosition,	updateName,	defaultColor,		hitColor}

	RegisterCollisionDetectionData({ "Body",	0.87f,	false });
	RegisterCollisionDetectionData({ "Head",	0.87f,	false,	{0.0f,0.0f,0.0f},"Body" });
	RegisterCollisionDetectionData({ "Left",	0.5f,	false });
	RegisterCollisionDetectionData({ "Right",	0.5f,	false });

#pragma endregion ----- 押し出し判定登録 -----

#pragma region ----- くらい判定登録 -----
	//	{名前、半径、	オフセット位置、ダメージ倍率、	更新名、	デフォルトカラー、	ヒットカラー}
	//	{name, radius,	offsetPos,		damage,			updateName,	defaultColor,		hitColor}

	//RegisterDamageDetectionData();

#pragma endregion ----- くらい判定登録 -----

#pragma region ----- 攻撃判定登録 -----
	//	{名前、半径、	オフセット位置、更新名、	デフォルトカラー、	ヒットカラー}
	//	{name, radius,	offsetPos,		updateName, defaultColor,		hitColor}

	//RegisterAttackDetectionData();
	
#pragma endregion ----- 攻撃判定登録 -----
}

//	当たり判定更新
void Drone::UpdateCollisions(const float& elapsedTime)
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
		//data.SetJointPosition(GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition()));
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
void Drone::DrawDebug()
{
	float scale = GetTransform()->GetScaleFactor();

	if (ImGui::TreeNode(u8"Drone ドローン"))
	{
		//GetTransform()->DrawDebug();
		Character::DrawDebug();

		ImGui::Checkbox("Invincible", &isInvincible_);			//	無敵フラグ設定
		ImGui::Checkbox("Bullet Launch ", &bulletLaunch_);		//	弾丸発射
		ImGui::Checkbox("Turn Action", &turnAction_);			//	旋回するかどうか
		
		ImGui::DragFloat("ScaleFactor", &scale,1.0f, -FLT_MAX, FLT_MAX);		//	スケール
		ImGui::DragFloat("TurnSpeed", &turnSpeed_, 1.0f, -FLT_MAX, FLT_MAX);	//	旋回速度
		ImGui::DragFloat("SerchRange", &searchRange_, 0.1f, -FLT_MAX, FLT_MAX);
		ImGui::TreePop();
	}
	BulletManager::Instance().DrawDebug();	//	弾丸ImGui

}