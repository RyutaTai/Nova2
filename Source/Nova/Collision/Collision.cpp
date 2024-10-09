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

////	レイとモデルの交差判定
//bool Collision::IntersectRayVsModel(
//	const DirectX::XMFLOAT3& start,
//	const DirectX::XMFLOAT3& end,
//	const Model* model,
//	HitResult& result)
//{
//	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
//	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
//	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
//	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);
//
//	//	ワールド空間のレイの長さ
//	DirectX::XMStoreFloat(&result.distance, WorldRayLength);
//
//	bool hit = false;
//	const ModelResource* resource = model->GetResource();
//	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
//	{
//		//	メッシュノード取得
//		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);
//
//		//	レイをワールド空間からローカル空間へ変換
//		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
//		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
//
//		DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
//		DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
//		DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
//		DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
//		DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);
//
//		//	レイの長さ
//		float neart;
//		DirectX::XMStoreFloat(&neart, Length);
//
//		//	三角形（面）との交差判定
//		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
//		const std::vector<UINT>indices = mesh.indices;
//
//		int materialIndex = -1;
//		DirectX::XMVECTOR HitPosition;
//		DirectX::XMVECTOR HitNormal;
//		for (const ModelResource::Subset& subset : mesh.subsets)
//		{
//			for (UINT i = 0; i < subset.indexCount; i += 3)
//			{
//				UINT index = subset.startIndex + i;
//
//				//	三角形の頂点を抽出
//				const ModelResource::Vertex& a = vertices.at(indices.at(index));
//				const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
//				const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));
//
//				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
//				DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
//				DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);
// 
//				//	三角形の三辺ベクトルを算出
//				DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
//				DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
//				DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);
//
//				//	三角形の法線ベクトルを算出
//				DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);
//
//				//	内積の結果がプラスならば裏向き(裏面の場合は衝突しないようにする)
//				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
//				float dot;
//				DirectX::XMStoreFloat(&dot, Dot);
//				if (dot >= 0)continue;
//
//				//	レイと平面の交点を算出
//				DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S);
//				DirectX::XMVECTOR X = DirectX::XMVectorDivide(DirectX::XMVector3Dot(N,SA),Dot);
//				float x;
//				DirectX::XMStoreFloat(&x, X);
//				if (x<.0f || x>neart)continue;	//交点までの距離が今までに計算した最近距離より大きいときはスキップ
//				DirectX::XMVECTOR P = DirectX::XMVectorAdd(S,DirectX::XMVectorMultiply(V, X));
//
//				//	交点が三角形の内側にあるか判定
//				//	１つ目
//				DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
//				DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
//				DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N);
//				float dot1;
//				DirectX::XMStoreFloat(&dot1, Dot1);
//				if (dot1 < 0.0f)continue;
//
//				//	２つ目
//				DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
//				DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
//				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N);
//				float dot2;
//				DirectX::XMStoreFloat(&dot2, Dot2);
//				if (dot2 < 0.0f)continue;
//
//				//	３つ目
//				DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
//				DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
//				DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N);
//				float dot3;
//				DirectX::XMStoreFloat(&dot3, Dot3);
//				if (dot3 < 0.0f)continue;
//
//				//	最近距離を更新
//				neart = x;
//
//				//	交点と法線を更新
//				HitPosition = P;
//				HitNormal = N;
//				materialIndex = subset.materialIndex;
//
//			}
//		}
//
//		if (materialIndex >= 0)
//		{
//			//	ローカル空間からワールド空間へ変換
//			DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
//			
//			DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
//			DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
//			float distance;
//			DirectX::XMStoreFloat(&distance, WorldCrossLength);
//
//			//	ヒット情報保存
//			if (result.distance > distance)
//			{
//				DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);
//
//				result.distance = distance;
//				result.materialIndex = materialIndex;
//				DirectX::XMStoreFloat3(&result.position, WorldPosition);
//				DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
//				hit = true;
//			}
//		}
//	}
//
//	return hit;
//}
