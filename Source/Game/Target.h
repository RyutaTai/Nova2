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
	DirectX::XMFLOAT3	origin_			= {};		//	索敵原点
	DirectX::XMFLOAT3	targetPos_		= {};		//	ターゲット位置
	bool				isTargetLocate_ = false;	//	ターゲットを発見しているか
	float				searchRange_	= 1.0f;		//	探索範囲

};

