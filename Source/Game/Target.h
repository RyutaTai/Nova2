#pragma once

#include <DirectXMath.h>

class Target
{
public:
	Target(){}
	~Target() {}

	void Update(const float& elapsedTime);

	void SetTargetPos(const DirectX::XMFLOAT3& targetPos) { targetPos_ = targetPos; }
	void SetTargetLocate(bool isTargetLocate) { isTargetLocate_ = isTargetLocate; }
	void SetSearchRange(const float& searchRange) { searchRange_ = searchRange; }

private:
	DirectX::XMFLOAT3	origin_			= {};		//	���G���_
	DirectX::XMFLOAT3	targetPos_		= {};		//	�^�[�Q�b�g�ʒu
	bool				isTargetLocate_ = false;	//	�^�[�Q�b�g�𔭌����Ă��邩
	float				searchRange_	= 1.0f;		//	�T���͈�

};

