#include "Collision.h"

#include <vector>
#include <wrl.h>
#include <iostream>
#include "../Resources/Model.h"

//	球と球の交差判定
bool Collision::IntersectSphereVsSphere(const DirectX::XMFLOAT3& positionA, float radiusA,
	const DirectX::XMFLOAT3& positionB, float radiusB,
	DirectX::XMFLOAT3& outPositionB)
{
	//	A->Bの単位ベクトルを算出
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);//ルートを取っていない状態（2乗）
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	//	距離判定
	float range = radiusA + radiusB;
	if (lengthSq > range*range)//	球の距離が(半径+半径)より大きかったら当たっていない
	{
		return false;
	}

	//	AがBを押し出す
	DirectX::XMVECTOR OutPositionB;
	Vec = DirectX::XMVector3Normalize(Vec);
	Vec = DirectX::XMVectorScale(Vec, range);
	OutPositionB = DirectX::XMVectorAdd(PositionA,Vec);
	DirectX::XMStoreFloat3(&outPositionB, OutPositionB);

	return true;
}

//	円柱と円柱の交差判定
bool Collision::IntersectCylinderVsCyliner(const DirectX::XMFLOAT3& positionA, float radiusA, float heightA,
	const DirectX::XMFLOAT3& positionB, float radiusB, float heightB,
	DirectX::XMFLOAT3& outPosition, bool outIsA)
{
	//	Aの足元がBの頭より上なら当たっていない
	if (positionA.y > positionB.y + heightB)
	{
		return false;
	}
	//	Aの頭がBの足元より下なら当たっていない
	if (positionA.y + heightA < positionB.y)
	{
		return false;
	}
	//	XZ平面での範囲チェック
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);//PositionAとBが逆だった
	DirectX::XMVECTOR Length = DirectX::XMVector3LengthSq(Vec);
	float length;
	DirectX::XMStoreFloat(&length, Length);
	float range = radiusA + radiusB;
	if (length>range*range)
	{
		return false;
	}

	if (outIsA)
	{
		//	BがAを押し出す
		DirectX::XMVECTOR OutPositionA;
		OutPositionA = DirectX::XMVectorSubtract(PositionB, DirectX::XMVectorScale(DirectX::XMVector3Normalize(Vec), range));
		DirectX::XMStoreFloat3(&outPosition, OutPositionA);
	}
	else
	{
		//	AがBを押し出す
		DirectX::XMVECTOR OutPositionB;
		OutPositionB = DirectX::XMVectorAdd(PositionA, DirectX::XMVectorScale(DirectX::XMVector3Normalize(Vec), range));
		DirectX::XMStoreFloat3(&outPosition, OutPositionB);
	}

	return true;
}

//	球と円柱の交差判定
bool Collision::IntersectSphereVsCylinder(const DirectX::XMFLOAT3& spherePosition, float sphereRadius,
	const DirectX::XMFLOAT3& cylinderPosition, float cylinderRadius, float cylinderHeight,
	DirectX::XMFLOAT3& outCylinderPosition)
{
	//	球の位置が円柱の位置より上なら当たっていない
	if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight)
	{
		return false;
	}
	//	球の位置が円柱の位置より下なら当たっていない
	if (spherePosition.y + sphereRadius < cylinderPosition.y)
	{
		return false;
	}

	//	A->Bの単位ベクトルを算出
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&spherePosition);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&cylinderPosition);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);//	ルートを取っていない状態（2乗）
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);
	//	距離判定
	float range = sphereRadius + cylinderRadius;
	if (lengthSq > range * range)//	球の距離が(半径+半径)より大きかったら当たっていない
	{
		return false;
	}

	return true;
}