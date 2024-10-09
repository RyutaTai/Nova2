#pragma once

#include "../Others/Transform.h"

class Camera
{
private:
	Camera();
	~Camera();

public:
	static Camera& Instance()
	{
		static Camera instance_;
		return instance_;
	}

	void Initialize();
	void Update(const float& elapsedTime);	//	�X�V����

	void NormalCamera(const float& elapsedTime);	//	�ʏ�J����

	// ���̊֐����R�[������΃J�����ړ����C�[�W���O�Ŏn�܂�܂��B
	//�������F�ڕW���W
	//�������F�ڕW�p�x
	//��O�����F���b�ňړ����邩
	bool LaunchCameraMove(DirectX::XMFLOAT3 targetEye_, DirectX::XMFLOAT3 targetAngle, float moveTime_);

	//	�f�o�b�O�p
	void DebugCamera(const float& elapsedTime);	//	�f�o�b�O�p�J�����X�V����
	void DrawDebug();						//	�f�o�b�O�`��
	void Reset();							//	���Z�b�g

	void SetPerspectiveFov();		//	�p�[�X�y�N�e�B�u�ݒ�
	void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);	//	�w�����������
	void SetEye(const DirectX::XMFLOAT3& eye)		{ this->eye_ = eye; }					//	���_�ݒ�
	void SetEyeX(const float& eyeX)					{ this->eye_.x = eyeX; }	
	void SetEyeY(const float& eyeY)					{ this->eye_.y = eyeY; }
	void SetEyeZ(const float& eyeZ)					{ this->eye_.z = eyeZ; }
	void SetTarget(const DirectX::XMFLOAT3& target) { this->focus_ = target; }				//	�^�[�Q�b�g�ʒu�ݒ�
	void SetRange(const float& range)				{ this->range_ = range; }				//	�J���������ݒ�
	void SetIsPose(bool isPose)						{ this->isPose_ = isPose; }

	Transform*					GetTransform()		{ return &transform_; }
	const DirectX::XMMATRIX		GetViewMatrix()const{ return viewMatrix_; }					//	�r���[�s��擾
	const DirectX::XMMATRIX		GetProjectionMatrix()const{ return projectionMatrix_; }		//	�v���W�F�N�V�����s��擾
	const DirectX::XMMATRIX		CalcViewProjectionMatrix();									//	�r���[�v���W�F�N�V�����s��擾	
	const DirectX::XMFLOAT3		GetEye()	const	{ return eye_; }						//	���_�擾
	const DirectX::XMFLOAT3		GetFocus()	const	{ return focus_; }						//	�����_�擾
	const DirectX::XMFLOAT3		GetUp()		const	{ return up_; }							//	�J����������擾
	const DirectX::XMFLOAT3		GetFront()	const	{ return front_; }						//	�J�����O�����擾
	const DirectX::XMFLOAT3		GetRight()	const	{ return right_; }						//	�J�����E�����擾
	const float					GetNearZ()  const	{ return nearZ_; }
	const float					GetFarZ()	const	{ return farZ_; }
	bool						GetIsPose()			{ return isPose_; }

	bool						IsCameraMove()		{ return cameraMove_; }					//	�J�������o�����ǂ���

private:
	Transform transform_;

	DirectX::XMMATRIX viewMatrix_;								//	�r���[�s��
	DirectX::XMMATRIX projectionMatrix_;						//	�v���W�F�N�V�����s��
	DirectX::XMMATRIX viewProjectionMatrix_;					//	�r���[�v���W�F�N�V�����s��
	DirectX::XMFLOAT3 eye_;										//	�J�����̎��_
	DirectX::XMFLOAT3 focus_		= { 0,-10,10 };				//	�J�����̒����_
	DirectX::XMFLOAT3 angle_		= { 0,0,0 };				//	�J�����̉�]�l
	DirectX::XMFLOAT3 eyeOffset_	= { 0,0,0 };				//	�J�����̎��_eye_�𓮂����Ƃ��̈ړ��l

	float fov_;													//	����p
	float range_			= 250.0f;							//	�J�����ƃ^�[�Q�b�g�̋��������߂�̂Ɏg��
	float rollSpeed_		= DirectX::XMConvertToRadians(45);	//	�J�����̉�]���x
	float maxAngleX_		= DirectX::XMConvertToRadians(45);	//	�J�����̉�]�l����
	float minAngleX_		= DirectX::XMConvertToRadians(-45);	//	�J�����̉�]�l����
	float moveSpeed_		=  2.0f;							//	�J�����̈ړ����x
	float maxEyeOffset_		=  2.0f;
	float minEyeOffset_		= -2.0f;
	float nearZ_			= 50.0f;
	float farZ_				= 400000.0f;

	DirectX::XMFLOAT3 up_;										//	�J�����̏����
	DirectX::XMFLOAT3 front_;									//	�J�����̑O����
	DirectX::XMFLOAT3 right_;									//	�J�����̉E����

	bool isPose_ = false;

	bool CameraMove(const float& elapsedTime);
	bool cameraMove_ = false;
	float moveTime_;
	float moveTimer_;
	DirectX::XMFLOAT3 moveTargetEye_;
	DirectX::XMFLOAT3 moveTargetAngle_;
	DirectX::XMFLOAT3 cashPos_;
	DirectX::XMFLOAT3 cashAngle_;

	//	�f�o�b�O�p
	bool isDebugCamera_ = false;

};

