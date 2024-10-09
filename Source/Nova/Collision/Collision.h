#pragma once

#include <DirectXMath.h>

//	�q�b�g����
struct HitResult
{
	DirectX::XMFLOAT3 position_ = { 0,0,0 };	//	���C�ƃ|���S���̌�_
	DirectX::XMFLOAT3 normal_	= { 0,0,0 };	//	�Փ˂����|���S���̖@���x�N�g��
	float distance_ = 0.0f;						//	���C�̎n�_�����_�܂ł̋���
	int materialIndex_;							//	�Փ˂����|���S���̃}�e���A���ԍ�
	DirectX::XMFLOAT3 rotation_ = { 0,0,0 };	//	��]��
};

//	�R���W����
class Collision
{
public:
	//	���Ƌ��̌�������
	static bool IntersectSphereVsSphere(const DirectX::XMFLOAT3& positionA, float radiusA,
		const DirectX::XMFLOAT3& positionB, float radiusB,
		DirectX::XMFLOAT3& outPositionB);

	//	�~���Ɖ~���̌�������
	static bool IntersectCylinderVsCyliner(const DirectX::XMFLOAT3& positionA, float radiusA, float heightA,
		const DirectX::XMFLOAT3& positionB, float radiusB, float heightB,
		DirectX::XMFLOAT3& outPosition, bool outIsA = false/*A��������B��������*/);

	//	���Ɖ~���̌�������
	static bool IntersectSphereVsCylinder(const DirectX::XMFLOAT3& spherePosition, float sphereRadius,
		const DirectX::XMFLOAT3& cylinderPosition, float cylinderRadius, float cylinderHeight,
		DirectX::XMFLOAT3& outCylinderPosition);

	////	���C�ƃ��f���̌�������
	//static bool IntersectRayVsModel(
	//	const DirectX::XMFLOAT3& start,
	//	const DirectX::XMFLOAT3& end,
	//	const Model* model,
	//	HitResult& result
	//);
};