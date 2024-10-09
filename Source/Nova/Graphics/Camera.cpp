#include "Camera.h"

#include <algorithm>

#include "../Core/Framework.h"
#include "../Input/Gamepad.h"
#include "../Others/MathHelper.h"
#include "../Others/Easing.h"
#include "../Input/Input.h"

//	�R���X�g���N�^
Camera::Camera()
{
}

//	�f�X�g���N�^
Camera::~Camera()
{
}

//	������
void Camera::Initialize()
{
	//	�V�e�B���f���`�掞
	//eye_	= { 0.0f,	200.0f,	500.0f };		//	�J�����̎��_
	//focus_	= { 0,		25.0f,	400.0f };	//	�J�����̒����_
	//up_		= { 0,		0.0f,	0.0f };		//	�J�����̏����
	//angle_	= { -0.20f,		0,		0 };	//	�J�����̉�]�l
	//fov_	= 60.0f;							//	����p

	////	ShadowMap�m�F�p
	//eye_	= { 0.0f, 10.0f, 100.0f };		//	�J�����̎��_
	//focus_	= { 0.0f, 25.0f,  50.0f };	//	�J�����̒����_
	//up_		= { 0.0f,  0.0f,   0.0f };	//	�J�����̏����
	//angle_	= { 0.5f,  0.0f,   0.0f };	//	�J�����̉�]�l
	//eyeOffset_ = { 1.0f,2.0f,0.0f };
	//fov_	= 60.0f;						//	����p
	//range_	= 15.0f;					//	�^�[�Q�b�g�ƃJ�����Ƃ̋���
	//nearZ_ = 0.01f;
	//farZ_ = 1000.0f;

	//	�ʏ���s�p
	eye_		= { 9.0f, 12.0f, -9.5f };		//	�J�����̎��_
	focus_		= { 0.0f, 5.0f,  0.0f };		//	�J�����̒����_
	up_			= { -0.3f,  0.8f,   0.3f };		//	�J�����̏����
	angle_		= { 0.5f,  -0.76f,   0.0f };	//	�J�����̉�]�l
	eyeOffset_	= { 1.0f,2.0f,0.0f };
	fov_		= 60.0f;						//	����p
	range_		= 15.0f;						//	�^�[�Q�b�g�ƃJ�����Ƃ̋���
	nearZ_		= 0.01f;
	farZ_		= 1000.0f;
}

//	�p�[�X�y�N�e�B�u�ݒ�
void Camera::SetPerspectiveFov()
{
    //	��ʃA�X�y�N�g��
    float aspectRatio = SCREEN_WIDTH / (float)SCREEN_HEIGHT;

    //	�v���W�F�N�V�����s��
    projectionMatrix_ = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov_), aspectRatio, nearZ_, farZ_);

	DirectX::XMVECTOR eye, focus, up;
	eye = DirectX::XMVectorSet(eye_.x, eye_.y, eye_.z, 1.0f);
	focus = DirectX::XMVectorSet(focus_.x,focus_.y,focus_.z, 1.0f);
	up = DirectX::XMVectorSet(up_.x, up_.y, up_.z, 0.0f);
	//	�J�����̎��_(�r���[�s��)
	viewMatrix_ = DirectX::XMMatrixLookAtLH(eye, focus, up);

}

//	�r���[�v���W�F�N�V�����s��Z�o
const DirectX::XMMATRIX Camera::CalcViewProjectionMatrix()
{
	viewProjectionMatrix_ = viewMatrix_* projectionMatrix_;
	return viewProjectionMatrix_;
}

//	�w�����������
void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	//	���_�A�����_�A���������r���[�s����쐬
	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	viewMatrix_ = View;

	//	�r���[���t�s�񉻂��A���[���h�s��ɖ߂�
	//	XMMatrixInverse�֐��ŋt�s������߂�
	DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, World);

	//	�J�����̕��������o��

	this->right_.x = world.m[0][0];
	this->right_.y = world.m[0][1];
	this->right_.z = world.m[0][2];

	this->up_.x = world.m[1][0];
	this->up_.y = world.m[1][1];
	this->up_.z = world.m[1][2];

	this->front_.x = world.m[2][0];
	this->front_.y = world.m[2][1];
	this->front_.z = world.m[2][2];
	this->GetTransform()->SetWorld(World);

	//	���_�A�����_��ۑ�
	this->eye_ = eye;
	this->focus_ = focus;

}

bool Camera::LaunchCameraMove(DirectX::XMFLOAT3 targetEye_, DirectX::XMFLOAT3 targetAngle, float moveTime_)
{
	if (cameraMove_)return false;

	moveTime_ = moveTime_;
	moveTimer_ = 0.0f;

	cashPos_ = eye_;
	cashAngle_ = angle_;
	moveTargetEye_ = targetEye_;
	moveTargetAngle_ = targetAngle;

	cameraMove_ = true;

	return true;
}

bool Camera::CameraMove(const float& elapsedTime)
{
	if (!cameraMove_)return false;

	if (moveTimer_ >= moveTime_)
	{
		eye_ = moveTargetEye_;
		angle_ = moveTargetAngle_;
		cameraMove_ = false;
		return false;
	}

	eye_.x = Easing::InSine(moveTimer_, moveTime_, moveTargetEye_.x, cashPos_.x);
	eye_.y = Easing::InSine(moveTimer_, moveTime_, moveTargetEye_.y, cashPos_.y);
	eye_.z = Easing::InSine(moveTimer_, moveTime_, moveTargetEye_.z, cashPos_.z);
	angle_.x = Easing::InSine(moveTimer_, moveTime_, moveTargetAngle_.x, cashAngle_.x);
	angle_.y = Easing::InSine(moveTimer_, moveTime_, moveTargetAngle_.y, cashAngle_.y);
	angle_.z = Easing::InSine(moveTimer_, moveTime_, moveTargetAngle_.z, cashAngle_.z);

	//float time = HighResolutionTimer::Instance().GetDeltaTime();
	moveTimer_ += elapsedTime;

    return true;
}

//	�X�V����
void Camera::Update(const float& elapsedTime)
{
	if (isDebugCamera_)DebugCamera(elapsedTime);
	else NormalCamera(elapsedTime);
}

//	�ʏ�J����
void Camera::NormalCamera(const float& elapsedTime)
{
	//#ifdef _DEBUG
//	// �J���������o�ňړ����͓����Ȃ�
//	if (!CameraMove(elapsedTime))
//	{
//		GamePad& gamePad = Input::Instance().GetGamePad();
//		float ax = gamePad.GetAxisRX();
//		float ay = gamePad.GetAxisRY();
//		//�J�����̉�]���x
//		float speed = rollSpeed_ * elapsedTime;
//
//		//�X�e�B�b�N�̓��͒l�ɍ��킹��X����Y������]
//		angle_.x -= ay * speed;
//		angle_.y += ax * speed;
//	}
//#else	//	RELEASE
//	CameraMove(elapsedTime);
//#endif // DEBUG

	//	�E�X�e�B�b�N�ŃJ������]
	if (!isPose_)
	{
		GamePad& gamePad = Input::Instance().GetGamePad();
		float ax = gamePad.GetAxisRX();
		float ay = gamePad.GetAxisRY();
		//	�J�����̉�]���x
		float speed = rollSpeed_ * elapsedTime;

		//	�X�e�B�b�N�̓��͒l�ɍ��킹��X����Y������]
		angle_.x -= ay * speed;
		angle_.y += ax * speed;
	}

	//	�J������]�l����]�s��ɕϊ�
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle_.x, angle_.y, angle_.z);

	//	��]�s�񂩂�O�����x�N�g�������o��
	//	Transform.r[2]�ōs��̂R�s�ڂ̃f�[�^�����o���Ă���
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	// TODO:SetLoolAt()���Ăяo���Ă�focus�̒l���ς��Ȃ��Bfocus��ς�����悤�ɂ���
	//		 eye���ς��Ȃ��Ƃ��������Ȃ邩��
	// TODO:�����̎������������C������(�@front�@���@this->front_�@�ɕς��Ă��Ⴄ�C������)�@2024/03/10
	this->eye_.x = this->focus_.x - (front.x * this->range_);
	this->eye_.y = this->focus_.y - (front.y * this->range_);
	this->eye_.z = this->focus_.z - (front.z * this->range_);

	//	�J�����̎��_�ƒ����_��ݒ�
	Camera::Instance().SetLookAt(eye_, focus_, DirectX::XMFLOAT3(0, 1, 0));

}

//	�f�o�b�O�p�J����(�܂��Ăяo���ĂȂ�)
//	��Update()�ɂ�����������DebugCamera()�Ɉڂ���
void Camera::DebugCamera(const float& elapsedTime)
{
#ifdef _DEBUG
	// �J���������o�ňړ����͓����Ȃ�
	if (!CameraMove(elapsedTime))
	{
		GamePad& gamePad = Input::Instance().GetGamePad();
		float RX = gamePad.GetAxisRX();
		float RY = gamePad.GetAxisRY();
		float LY = gamePad.GetAxisLY();

		//	�J�����̉�]���x
		float aSpeed = rollSpeed_ * elapsedTime;
		float moveSpeed = moveSpeed_ * elapsedTime;

		//	�J�����ړ�(X���AY��)
		//	����Shift�L�[�ƉE�X�e�B�b�N(IJKL�L�[)�����ꂩ�������Ă���Ƃ��A
		//	�J�����̎��_���X�e�B�b�N�̓��͒l�ɍ��킹��X,Y�������Ɉړ�
		//if ((gamePad.GetButtonDown()&GamePad::BTN_LEFT_TRIGGER) && (RX != 0 || RY != 0))
		if ((GetAsyncKeyState(VK_LSHIFT)) && (RX != 0 || RY != 0))
		{
			eyeOffset_.x -= RX * moveSpeed;
			eyeOffset_.y += RY * moveSpeed;
		}
		else	//	�J������]�@�E�X�e�B�b�N�̓��͒l�ɍ��킹��X����Y������](�����_����]������)
		{
			angle_.x += RY * aSpeed;
			angle_.y += RX * aSpeed;
			eyeOffset_.x = 0;	//	X,Y���̃J�����ړ��l�����Z�b�g
			eyeOffset_.y = 0;
		}

		//	�J�����ړ�(Z��)�@���X�e�B�b�N(W,A�L�[)�̓��͒l�ɍ��킹��Z�������Ɉړ�
		//	���͂��Ȃ������烊�Z�b�g
		if (gamePad.GetAxisLY() != 0)//	���X�e�B�b�N
		{
			eyeOffset_.z -= LY * moveSpeed;
		}
		else	eyeOffset_.z = 0;

#if 1 //	������O���ƌ����Ă�����ɍs���Ȃ��Ȃ�
		//////////////////////////////////////////////////////////////////////////////////////////////////
		DirectX::XMFLOAT3 cameraRight = this->GetRight();
		DirectX::XMFLOAT3 cameraFront = this->GetFront();
		DirectX::XMFLOAT3 cameraUp = this->GetUp();

		//	�J�����E�����x�N�g����P�ʃx�N�g���ɕϊ�
		float Rlength;
		DirectX::XMStoreFloat(&Rlength, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&cameraRight)));
		float cameraRightLength = DirectX::XMVectorGetX(DirectX::XMVectorSqrt(DirectX::XMLoadFloat3(&cameraRight)));
		if (cameraRightLength > 0.0f)
		{
			//	�P�ʃx�N�g����
			DirectX::XMVECTOR cameraRightVec = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&cameraRight));
			DirectX::XMStoreFloat3(&cameraRight, cameraRightVec);
		}

		//	�J�����O�����x�N�g����P�ʃx�N�g���ɕϊ�
		float Zlength;
		DirectX::XMStoreFloat(&Zlength, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&cameraFront)));
		float cameraFrontLength = DirectX::XMVectorGetX(DirectX::XMVectorSqrt(DirectX::XMLoadFloat3(&cameraFront)));
		if (cameraFrontLength > 0.0f)
		{
			//	�P�ʃx�N�g����
			DirectX::XMVECTOR cameraFrontVec = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&cameraFront));
			DirectX::XMStoreFloat3(&cameraFront, cameraFrontVec);
		}

		//	�J����������x�N�g����P�ʃx�N�g���ɕϊ�
		float Ulength;
		DirectX::XMStoreFloat(&Ulength, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&cameraUp)));
		float cameraUpLength = DirectX::XMVectorGetX(DirectX::XMVectorSqrt(DirectX::XMLoadFloat3(&cameraUp)));
		if (cameraUpLength > 0.0f)
		{
			//	�P�ʃx�N�g����
			DirectX::XMVECTOR cameraUpVec = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&cameraUp));
			DirectX::XMStoreFloat3(&cameraUp, cameraUpVec);
		}

		//	�X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
		//	�X�e�B�b�N�������͒l���J�����O�����ɔ��f���A
		//	�i�s�x�N�g�����v�Z����
		eyeOffset_.x = cameraFront.x * LY * moveSpeed_ + cameraRight.x * RX * moveSpeed_ + cameraUp.x * RY * moveSpeed_;
		eyeOffset_.y = cameraFront.y * LY * moveSpeed_ + cameraRight.y * RX * moveSpeed_ + cameraUp.y * RY * moveSpeed_;
		eyeOffset_.z = cameraFront.z * LY * moveSpeed_ + cameraRight.z * RX * moveSpeed_ + cameraUp.z * RY * moveSpeed_;
		////////////////////////////////////////////////////////////////////////////////////////////
#endif

	//	�J�����ړ�(X���AY��)
	//	����Shift�L�[�ƉE�X�e�B�b�N(IJKL�L�[)�����ꂩ�������Ă���Ƃ��A
	//	�J�����̎��_���X�e�B�b�N�̓��͒l�ɍ��킹��X,Y�������Ɉړ�
		//if ((gamePad.GetButtonDown()&GamePad::BTN_LEFT_TRIGGER) && (RX != 0 || RY != 0))
		if ((GetAsyncKeyState(VK_LSHIFT)) && (RX != 0 || RY != 0))
		{
			eyeOffset_.x -= RX * moveSpeed;
			eyeOffset_.y += RY * moveSpeed;
		}
		else	//	�J������]�@�E�X�e�B�b�N�̓��͒l�ɍ��킹��X����Y������](�����_����]������)
		{
			angle_.x += RY * aSpeed;
			angle_.y += RX * aSpeed;
			eyeOffset_.x = 0;	//	X,Y���̃J�����ړ��l�����Z�b�g
			eyeOffset_.y = 0;
		}

		//	�J�����ړ�(Z��)�@���X�e�B�b�N(W,A�L�[)�̓��͒l�ɍ��킹��Z�������Ɉړ�
		//	���͂��Ȃ������烊�Z�b�g
		if (gamePad.GetAxisLY() != 0)//	���X�e�B�b�N
		{
			eyeOffset_.z -= LY * moveSpeed;
		}
		else	eyeOffset_.z = 0;

	}
#else	//	RELEASE
	CameraMove(elapsedTime);

#endif // DEBUG

	//	�J������]�l����]�s��ɕϊ�
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle_.x, angle_.y, angle_.z);

	//	��]�s�񂩂�O�����x�N�g�������o��
	//	Transform.r[2]�ōs��̂R�s�ڂ̃f�[�^�����o���Ă���
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	// TODO:SetLoolAt()���Ăяo���Ă�focus�̒l���ς��Ȃ��Bfocus��ς�����悤�ɂ���
	//		 eye���ς��Ȃ��Ƃ��������Ȃ邩��
	focus_.x = eye_.x - (front.x * range_);
	focus_.y = eye_.y - (front.y * range_);
	focus_.z = eye_.z - (front.z * range_);

	eye_ = eye_ + eyeOffset_;	//	�J�������_�̍X�V

	//	�J�����̎��_�ƒ����_��ݒ�
	Camera::Instance().SetLookAt(eye_, focus_, DirectX::XMFLOAT3(0, 1, 0));
}

//	���Z�b�g
void Camera::Reset()
{
	eye_		= { 0.0f,	10.0f,	100.0f };	//	�J�������_
	eyeOffset_	= { 1.0f,	2.0f,	0.0f };		//	�J�������_�␳�l
	focus_		= { 0.0f,	25.0f,	50.0f };	//	�J�����̒����_
	up_			= { 0.0f,	0.0f,	0.0f };		//	�J�����̏����
	angle_		= { 0.0f,	0.0f,	0.0f };		//	�J�����̉�]�l
	fov_		= 60.0f;						//	����p
}

//	�f�o�b�O�`��
void Camera::DrawDebug()
{
	DirectX::XMFLOAT3 right = this->GetTransform()->CalcRight();
	DirectX::XMFLOAT3 forward = this->GetTransform()->CalcForward();
	
	if (ImGui::TreeNode(u8"Camera�J����"))
	{
		ImGui::Checkbox("DebugCamera", &isDebugCamera_);	//	�f�o�b�O�J�����؂�ւ�
		GetTransform()->DrawDebug();
		ImGui::DragFloat	("NearZ",		&nearZ_,		1.0f,	FLT_MIN,	FLT_MAX);	//	Near
		ImGui::DragFloat	("FarZ",		&farZ_,			1.0f,	FLT_MIN,	FLT_MAX);	//	Far
		ImGui::DragFloat	("MoveSpeed",	&moveSpeed_,	1.0f,	-FLT_MAX,	FLT_MAX);	//	�ړ����x
		ImGui::DragFloat3	("Eye",			&eye_.x,		0.01f,	-FLT_MAX,	FLT_MAX);	//	�J�������_
		ImGui::DragFloat3	("Focus",		&focus_.x,		0.01f,	-FLT_MAX,	FLT_MAX);	//	�����_
		ImGui::DragFloat3	("Right",		&right.x,		0.01f,	-FLT_MAX,	FLT_MAX);	//	�E����
		ImGui::DragFloat3	("Up",			&up_.x,			0.01f,	-FLT_MAX,	FLT_MAX);	//	�����
		ImGui::DragFloat3	("Forward",		&forward.x,		0.01f,	-FLT_MAX,	FLT_MAX);	//	�O����
		ImGui::DragFloat3	("Angle",		&angle_.x,		0.01f,	-FLT_MAX,	FLT_MAX);	//	��]�l
		ImGui::DragFloat3	("EyeOffset",	&eyeOffset_.x,	0.001f,	-FLT_MAX,	FLT_MAX);	//	�J�������_�␳�l
		ImGui::DragFloat	("Range",		&range_,		0.1f,	FLT_MIN,	FLT_MAX);	//	�Ԋu
		if (ImGui::Button	("Reset"))
		{
			Reset();
		}
		ImGui::TreePop();
	}
}
