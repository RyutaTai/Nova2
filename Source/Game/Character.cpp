#include "Character.h"

#include "../Nova/Core/Framework.h"
#include "Character.h"
#include "../Nova/Others/MathHelper.h"
#include "Stage.h"

//	コンストラクタ
Character::Character(const std::string& filename, const std::string& rootNodeName)
{
	//	モデル読み込み
	gltfModelResource_ = ResourceManager::Instance().LoadGltfModelResource(filename, rootNodeName);
}

//	ベロシティ更新
void Character::UpdateVelocity(const float& elapsedTime)
{
	//#if 0 //	キャラの前方向に移動させる処理
	//	DirectX::XMFLOAT3 front = this->GetTransform()->CalcForward();	//	キャラクターの前方向取得
	//	DirectX::XMVECTOR frontV = DirectX::XMLoadFloat3(&front);
	//	frontV = DirectX::XMVector3Normalize(frontV);
	//	DirectX::XMStoreFloat3(&front, frontV);
	//
	//	float range = 10.0f;
	//	velocity_ = front * range * Framework::tictoc_.GetDeltaTime();
	//	this->GetTransform()->AddPosition(velocity_);
	//#endif

#if 0
	float vecLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&moveVec_)));
	if (vecLength > 0)
	{
		velocity_ = moveVec_ * moveSpeed_ * elapsedTime;
	}
#else
	velocity_ = moveVec_ * moveSpeed_ * elapsedTime + acceleration_ * elapsedTime;

#endif
}

//	ベロシティ加算
void Character::AddVelocity(const DirectX::XMFLOAT3& addVelocity,const float& elapsedTime)
{
	DirectX::XMVECTOR Velocity = DirectX::XMLoadFloat3(&velocity_);
	DirectX::XMVECTOR AddVelocity = DirectX::XMLoadFloat3(&addVelocity);
	Velocity = DirectX::XMVectorAdd(Velocity, AddVelocity);
	//Velocity = DirectX::XMVectorScale(Velocity, elapsedTime);
	DirectX::XMStoreFloat3(&velocity_, Velocity);
}

void Character::AddVelocityY(const float& addVelocityY, const float& elapsedTime)
{
	velocity_.y += addVelocityY * elapsedTime;
}

void Character::AddVelocityXZ(const float& addVelocityX, const float& addVelocityZ, const float& elapsedTime)
{
	velocity_.x += addVelocityX * elapsedTime;
	velocity_.z += addVelocityZ * elapsedTime;
}

//	XZ方向のVelocityのみに引数の数を掛ける
void Character::MultiplyVelocityXZ(const float& multiplyVelocity, const float& elapsedTime)
{
	velocity_.x *= multiplyVelocity * elapsedTime;
	velocity_.z *= multiplyVelocity * elapsedTime;
}

//	加速度加算
void Character::AddAcceleration(const DirectX::XMFLOAT3& addAcceleration, const float& elapsedTime)
{
	DirectX::XMVECTOR Acceleration = DirectX::XMLoadFloat3(&acceleration_);
	DirectX::XMVECTOR AddAcceleration = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&addAcceleration), elapsedTime);
	Acceleration = DirectX::XMVectorAdd(Acceleration, AddAcceleration);
	DirectX::XMStoreFloat3(&acceleration_, Acceleration);
}

void Character::AddAccelerationY(const float& addAccelerationY, const float& elapsedTime)
{
	acceleration_.y += addAccelerationY * elapsedTime;
}

void Character::AddAccelerationXZ(const float& addAccelerationX, const float& addAccelerationZ, const float& elapsedTime)
{
	acceleration_.x += addAccelerationX * elapsedTime;
	acceleration_.z += addAccelerationZ * elapsedTime;
}

void Character::AddMoveSpeed(const float& addMoveSpeed, const float& elapsedTime)
{
	//	最大スピードを超えていない場合のみ加算処理
	if (moveSpeed_ < MOVE_SPEED)
	{
		moveSpeed_ += addMoveSpeed * elapsedTime;
	}
	else if (MOVE_SPEED >= moveSpeed_)
	{
		moveSpeed_ = MOVE_SPEED;
	}
}

//	移動処理
void Character::Move(const float& elapsedTime)
{
	this->GetTransform()->AddPosition(velocity_);
}

//	旋回処理
void Character::Turn(const float& elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	//	進行ベクトルがゼロベクトルの場合は処理する必要なし
	float length;
	length = sqrtf(vx * vx + vz * vz);

	if (length <= 0)
	{
		return;
	}

	//	進行ベクトルを単位ベクトル化
	vx /= length;
	vz /= length;

	//	自身の回転値から前方向を求める
	float angleY = this->GetTransform()->GetRotationY();
	float frontX = sinf(angleY);
	float frontZ = cosf(angleY);

	//	左右判定を行うために２つの単位ベクトルの外積を計算する
	float cross = (frontZ * vx) - (frontX * vz);

	//	回転角を求めるため、２つの単位ベクトルの内積を計算する
	float dot = (frontX * vx) + (frontZ * vz);

	//	内積値は-1.0～1.0で表現されている。
	//	２つの単位ベクトルの角度が小さいほど
	//	1.0に近づくという性質を利用して回転速度を調整する
	float rot = 1.0 - dot;
	if (rot > speed)rot = speed;

	//	2Dの外積値が正の場合か負の場合によって左右判定が行える
	//	左右判定を行うことによって左右回転を選択する
	//	正の場合は右、負の場合は左
	if (cross < 0.0f)//	左
	{
		this->GetTransform()->AddRotationY(-rot);
	}
	else//	右
	{
		this->GetTransform()->AddRotationY(rot);
	}
}

//	アニメーション再生
void Character::PlayAnimation(const int& index, const bool& loop, const float& speed, const float& blendTime,const float& startFrame)
{
	gltfModelResource_->PlayAnimation(index, loop, speed, blendTime, startFrame);
}

//	アニメーション更新処理
void Character::UpdateAnimation(const float& elapsedTime)
{
	gltfModelResource_->UpdateAnimation(elapsedTime);
}

//	アニメーション再生中かどうか
bool Character::IsPlayAnimation()const
{
	return gltfModelResource_->IsPlayAnimation();
}

//	ピクセルシェーダー設定
void Character::SetPixelShader(const char* csoName)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	Graphics::Instance().GetShader()->CreatePsFromCso(device, csoName, pixelShader_.ReleaseAndGetAddressOf());
	gltfModelResource_->SetPixelShader(pixelShader_.Get());
}

//	ジョイントポジション取得
DirectX::XMFLOAT3 Character::GetJointPosition(const std::string& meshName, const std::string& boneName, const DirectX::XMFLOAT4X4& transform)
{
	return gltfModelResource_->GetJointPosition(meshName, boneName, transform);
}

DirectX::XMFLOAT3 Character::GetJointPosition(size_t nodeIndex, const DirectX::XMFLOAT4X4& transform)
{
	return gltfModelResource_->GetJointPosition(nodeIndex, transform);
}

//	HP減少
void Character::SubtractHp(int hp)
{
	if (isInvincible_ == false)	//	無敵じゃなかったらHP減少
	{
		hp_ -= hp;
		if (hp_ <= 0)
		{
			hp_ = 0;
		}
	}
}

//	アニメーション追加
void Character::AppendAnimation(const std::string& filename)
{
	gltfModelResource_->AppendAnimation(filename);
}

//	描画処理
void Character::Render()
{
	gltfModelResource_->Render(GetTransform()->CalcWorld());
}

//	デバッグ描画
void Character::DrawDebug()
{
	gltfModelResource_->DrawDebug();
	GetTransform()->DrawDebug();

	ImGui::DragInt("HP", &hp_, 1.0f, 0, INT_MAX);									//	HP
	ImGui::DragFloat3("Velocity", &velocity_.x, 0.01f, -FLT_MAX, FLT_MAX);			//	移動速度
	ImGui::DragFloat3("Acceleration", &acceleration_.x, 0.01f, -FLT_MAX, FLT_MAX);	//	加速度
	ImGui::DragFloat3("moveVec", &moveVec_.x, 0.01f, -FLT_MAX, FLT_MAX);			//	移動ベクトル
	ImGui::DragFloat("Height", &height_, 0.01f, -FLT_MAX, FLT_MAX);					//	高さ
	ImGui::DragFloat("Radius", &radius_, 0.01f, -FLT_MAX, FLT_MAX);					//	半径

}

