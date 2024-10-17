#pragma once

#include "../Nova/Object/GameObject.h"

class Character :public GameObject
{
public:
	Character(const std::string& filename, const std::string& rootNodeName = "root");
	virtual ~Character() {}

	virtual void Initialize() = 0;
	virtual void Update(const float& elapsedTime) = 0;
	virtual bool RayVsVertical(const float& elapsedTime) = 0;		//	�X�e�[�W�Ƃ̓����蔻��
	virtual bool RayVsHorizontal(const float& elapsedTime) = 0;
	void Render()override;
	//virtual void Render(const float& scale = 1.0f, const float& animationSpeed = 1.0f);

	virtual void DrawDebug();

	void UpdateVelocity(const float& elapsedTime);
	void AddVelocity(const DirectX::XMFLOAT3& addVelocity,const float& elapsedTime);
	void AddVelocityY(const float& addVelocityY, const float& elapsedTime);
	void AddVelocityXZ(const float& addVelocityX, const float& addVelocityZ, const float& elapsedTime);
	void MultiplyVelocityXZ(const float& multiplyVelocity, const float& elapsedTime);
	void AddAcceleration(const DirectX::XMFLOAT3& addAcceleration, const float& elapsedTime);
	void AddAccelerationY(const float& addAccelerationY, const float& elapsedTime);
	void AddAccelerationXZ(const float& addAccelerationX, const float& addAccelerationZ, const float& elapsedTime);
	void AddMoveSpeed(const float& addMoveSpeed, const float& elapsedTime);
	virtual void Move(const float& elpasedTime);
	virtual void Turn(const float& elapsedTime, float vx, float vz, float speed);

	void PlayAnimation(const int& index, const bool& loop = false, const float& speed = 1.0f, const float blendTime = 1.0f, const float cutTime = 0.0f);
	void UpdateAnimation(const float& elapsedTime);
	bool IsPlayAnimation()const;

	void SetVelocity(const DirectX::XMFLOAT3& velocity)			{ velocity_ = velocity; }
	void SetAcceleration(const DirectX::XMFLOAT3& acceleration) { acceleration_ = acceleration; }
	void SetMoveSpeed(const float& moveSpeed)					{ moveSpeed_ = moveSpeed; }
	void SetPixelShader(const char* csoName);		//	�s�N�Z���V�F�[�_�[�ݒ�
	void SetHp(int hp)		{ hp_ = hp; }
	void SubtractHp(int hp);
	void SetIsInvincible(const bool& isInvincible) { isInvincible_ = isInvincible; }
	void SetInvincibleTimer(const float& invincibleTimer) { invincibleTimer_ = invincibleTimer; }

	const int				GetHp()				const	{ return hp_; }
	const float				GetRadius()			const	{ return radius_; }
	const float				GetHeight()			const	{ return height_; }
	const DirectX::XMFLOAT3 GetVelocity()		const	{ return velocity_; }
	const DirectX::XMFLOAT3 GetAcceleration()	const	{ return acceleration_; }
	const float				GetMoveSpeed()		const	{ return moveSpeed_; }
	const bool				IsInvincible()		const	{ return isInvincible_; }
	const float				GetInvincibleTimer()const	{ return invincibleTimer_; }

	int GetCurrentAnimNum();
	DirectX::XMFLOAT3 GetJointPosition(const std::string& meshName, const std::string& boneName, const DirectX::XMFLOAT4X4& transform);											//	�W���C���g�|�W�V�����擾
	DirectX::XMFLOAT3 GetJointPosition(size_t nodeIndex, const DirectX::XMFLOAT4X4& transform);

protected:
	DirectX::XMFLOAT3	velocity_ = {};		//	�ړ����x
	DirectX::XMFLOAT3	acceleration_ = {};	//	�����x(0�Ȃ瓙�������^��)
	DirectX::XMFLOAT3	moveVec_ = {};		//	�ړ��x�N�g��

	float				radius_ = 30.0f;	//	���a
	float				height_ = 195.0f;	//	����

	int					hp_ = 100;											//	���݂�HP
	float				animationSpeed_ = 1.0f;								//	�A�j���[�V�������Đ����鑬��
	float				turnSpeed_		= DirectX::XMConvertToRadians(720);	//	���񂷂鑬��
	float				moveSpeed_		= 2.0f;								//	�ړ����鑬��
	bool				isInvincible_	= false;							//	���G���ǂ���
	float				invincibleTimer_ = 0.0f;							//	���G����

private:
	const float MOVE_SPEED = 5.5f;		//	�ő�̑���

private:
	std::shared_ptr <GltfModel>					gltfModelResource_;		//	Gltf���f��
	Microsoft::WRL::ComPtr <ID3D11PixelShader>	pixelShader_;

};

