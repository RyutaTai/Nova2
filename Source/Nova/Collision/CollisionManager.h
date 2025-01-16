#pragma once

#include <DirectXMath.h>

class CollisionManager
{
public:
	//	”»’è‚Ìí—Ş
	enum class CollisionType
	{
		Attack,
		Max
	};

private:
	CollisionManager(){}
	~CollisionManager() {}

public:
	static CollisionManager& Instance()
	{
		static CollisionManager instance;
		return instance;
	}

	void Initialize();
	void Update(const float& elapsedTime);
	void Finalize();
	void DrawDebug();

	//	‹…‚Æ‹…‚ÌŒğ·”»’è(‰Ÿ‚µo‚µ‚ ‚è)
	bool IntersectSphereVsSphere(const DirectX::XMFLOAT3& positionA, const float& radiusA,
		const DirectX::XMFLOAT3& positionB, const float& radiusB,
		DirectX::XMFLOAT3& outPositionB);
	//	‹…‚Æ‹…‚ÌŒğ·”»’è(‰Ÿ‚µo‚µ‚È‚µ)
	bool IntersectSphereVsSphere(const DirectX::XMFLOAT3& positionA, const float& radiusA,
		const DirectX::XMFLOAT3& positionB, const float& radiusB);

	//	‰~’Œ‚Æ‰~’Œ‚ÌŒğ·”»’è
	bool IntersectCylinderVsCyliner(const DirectX::XMFLOAT3& positionA, const float& radiusA, const float& heightA,
		const DirectX::XMFLOAT3& positionB, const float& radiusB,const float& heightB,
		DirectX::XMFLOAT3& outPosition, const bool& outIsA = false/*A‚ğ‰Ÿ‚·‚©B‚ğ‰Ÿ‚·‚©*/);

	//	‹…‚Æ‰~’Œ‚ÌŒğ·”»’è
	bool IntersectSphereVsCylinder(const DirectX::XMFLOAT3& spherePosition, const float& sphereRadius,
		const DirectX::XMFLOAT3& cylinderPosition, const float& cylinderRadius, const float& cylinderHeight,
		DirectX::XMFLOAT3& outCylinderPosition);

private:


};

