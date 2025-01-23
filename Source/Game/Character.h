#pragma once

#include "DirectXMath.h"
#include "../Nova/Resources/GltfModel.h"
#include "../Nova/Collision/CollisionData.h"

class Character
{
public:
	Character(const std::string& filename, const std::string& rootNodeName = "root");
	virtual ~Character() {}

	virtual void Initialize() = 0;
	virtual void Update(const float& elapsedTime) = 0;
	virtual bool RayVsVertical(const float& elapsedTime) = 0;		//	ステージとの当たり判定
	virtual bool RayVsHorizontal(const float& elapsedTime) = 0;
	virtual void Render();
	virtual void DrawDebug();	//	デバッグ描画

	//	----- 速さ -----
	void AddMoveSpeed(const float& addMoveSpeed, const float& elapsedTime);
	void SetMoveSpeed(const float& moveSpeed){ moveSpeed_ = moveSpeed; }
	const float	GetMoveSpeed()const { return moveSpeed_; }

	//	----- 速度 -----
	void UpdateVelocity(const float& elapsedTime);
	void AddVelocity(const DirectX::XMFLOAT3& addVelocity,const float& elapsedTime);
	void AddVelocityY(const float& addVelocityY, const float& elapsedTime);
	void AddVelocityXZ(const float& addVelocityX, const float& addVelocityZ, const float& elapsedTime);
	void MultiplyVelocityXZ(const float& multiplyVelocity, const float& elapsedTime);
	void SetVelocity(const DirectX::XMFLOAT3& velocity)			{ velocity_ = velocity; }
	const DirectX::XMFLOAT3 GetVelocity()const { return velocity_; }
	
	//	----- 加速度 -----
	void AddAcceleration(const DirectX::XMFLOAT3& addAcceleration, const float& elapsedTime);
	void AddAccelerationY(const float& addAccelerationY, const float& elapsedTime);
	void AddAccelerationXZ(const float& addAccelerationX, const float& addAccelerationZ, const float& elapsedTime);
	void SetAcceleration(const DirectX::XMFLOAT3& acceleration) { acceleration_ = acceleration; }
	const DirectX::XMFLOAT3 GetAcceleration()const { return acceleration_; }
	
	//	----- 移動 -----
	virtual void Move(const float& elpasedTime);
	virtual void Turn(const float& elapsedTime, float vx, float vz, float speed);

	//	----- アニメーション -----
	void		PlayAnimation(const int& index, const bool& loop = false, const float& blendTime = 1.0f, const float& startFrame = 0.0f, const float& animSpeed = 1.0f);
	void		UpdateAnimation(const float& elapsedTime);
	void		AppendAnimation(const std::string& filename);
	bool		IsPlayAnimation()const;
	void		SetAnimationSpeed(const float& animationSpeed)			{ gltfModelResource_->SetAnimationSpeed(animationSpeed); }
	float const	GetCurrentAnimationSeconds()const{ return gltfModelResource_->GetCurrentAnimationSeconds(); }		//	現在再生中ののアニメーション再生時間取得
	int			GetCurrentAnimNum()			const{ return gltfModelResource_->GetCurrentAnimNum(); }				//	現在再生中のアニメーション番号取得

	//	-----	ピクセルシェーダー -----
	void SetPixelShader(const char* csoName);
	
	//	----- HP -----
	void		SubtractHp(const int& hp);
	void		SetHp(const int& hp){ hp_ = hp; }
	const int	GetHp()const { return hp_; }

	//	----- 無敵処理 -----
	void		SetIsInvincible(const bool& isInvincible)		{ isInvincible_ = isInvincible; }
	const bool	IsInvincible()const								{ return isInvincible_; }
	void		SetInvincibleTimer(const float& invincibleTimer){ invincibleTimer_ = invincibleTimer; }
	const float	GetInvincibleTimer()const						{ return invincibleTimer_; }

	//	----- ジョイントポジション -----
	DirectX::XMFLOAT3 GetJointPosition(const std::string& nodeName, const DirectX::XMFLOAT3& offsetPos = {});											//	ジョイントポジション取得
	DirectX::XMFLOAT3 GetJointPosition(const size_t& nodeIndex, const DirectX::XMFLOAT3& offsetPos = {});

	//	----- ルートモーション -----
	const int GetNodeIndex(const std::string& nodeName) { return gltfModelResource_->GetNodeIndex(nodeName); }
	void RootMotion() { gltfModelResource_->RootMotion(GetTransform()->GetScaleFactor()); }
	void SetRootJointIndex(const int& index) { gltfModelResource_->SetRootJointIndex(index); }
	void SetUseRootMotion(const bool& useRootMotion) { gltfModelResource_->SetUseRootMotion(useRootMotion); }

	//	----- Collision -----
	virtual void RegisterCollisionData() = 0;
	virtual void UpdateCollisions(const float& elapsedTime);
	void CollisionCharacterVsStage();
	
	//	----- 攻撃判定 -----
	void RegisterAttackDetectionData(const AttackDetectionData& data);
	const int GetAttackDetectionDataCount() const { return static_cast<int>(attackDetectionData_.size()); }
	std::vector<AttackDetectionData> GetAttackDetectionData() { return attackDetectionData_; }
	AttackDetectionData&	GetAttackDetectionData(const std::string& name);
	AttackDetectionData&	GetAttackDetectionData(const int& index);
	const float				GetRadius()	const{ return radius_; }
	const float				GetHeight()	const{ return height_; }

	//	----- くらい判定 -----
	void RegisterDamageDetectionData(const DamageDetectionData& data);
	const int GetDamageDetectionDataCount() const { return static_cast<int>(damageDetectionData_.size()); }
	std::vector<DamageDetectionData> GetDamageDetectionData() { return damageDetectionData_; }
	DamageDetectionData& GetDamageDetectionData(const std::string& name);
	DamageDetectionData& GetDamageDetectionData(const int& index);

	//	----- 押し出し判定 -----
	void RegisterCollisionDetectionData(const CollisionDetectionData& data);
	const int GetCollisionDetectionDataCount() const { return static_cast<int>(collisionDetectionData_.size()); }
	std::vector<CollisionDetectionData> GetCollisionDetectionData() { return collisionDetectionData_; }
	CollisionDetectionData& GetCollisionDetectionData(const std::string& name);
	CollisionDetectionData& GetCollisionDetectionData(const int& index);

	//	----- 死亡フラグ -----
	void SetIsDead(const bool& isDead) { isDead_ = isDead; }
	const bool IsDead()const { return isDead_; }

	//	----- 攻撃力 -----
	void SetAttackPower(const float& attackPower) { attackPower_ = attackPower; }
	const float GetAttackPower()const { return attackPower_; }

	Transform* GetTransform() { return gltfModelResource_->GetTransform(); }

protected:
	//	----- 移動 -----
	DirectX::XMFLOAT3	velocity_		= {};	//	移動速度
	DirectX::XMFLOAT3	acceleration_	= {};	//	加速度(0なら等速直線運動)
	DirectX::XMFLOAT3	moveVec_		= {};	//	移動ベクトル
	float				turnSpeed_		= DirectX::XMConvertToRadians(720);	//	旋回する速さ
	float				moveSpeed_		= 2.0f;								//	移動する速さ

	//	----- 無敵処理 -----
	bool	isInvincible_	= false;		//	無敵かどうか
	float	invincibleTimer_ = 0.0f;		//	無敵時間

	//	----- Collision -----
	std::vector<AttackDetectionData>	attackDetectionData_;		//	攻撃判定用
	std::vector<DamageDetectionData>	damageDetectionData_;		//	くらい判定
	std::vector<CollisionDetectionData>	collisionDetectionData_;	//	押し出し判定用
	float	radius_ = 30.0f;	//	半径
	float	height_ = 195.0f;	//	高さ

	int		hp_ = 100;				//	現在のHP
	bool	isDead_ = false;		//	死亡フラグ
	float	attackPower_ = 0.0f;	//	攻撃力

private:
	const float MOVE_SPEED = 10.0f;	//	最大の速さ

private:
	std::shared_ptr <GltfModel>					gltfModelResource_;		//	Gltfモデル
	Microsoft::WRL::ComPtr <ID3D11PixelShader>	pixelShader_;

};

