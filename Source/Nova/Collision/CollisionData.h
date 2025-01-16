#pragma once

#include <DirectXMath.h>
#include <string>

//	球判定用データ
struct SphereData
{
public:
	SphereData(const std::string& name,const DirectX::XMFLOAT3& jointPos,const DirectX::XMFLOAT3& offsetPos,
		const float& radius,const DirectX::XMFLOAT4& defaultColor,const DirectX::XMFLOAT4& hitColor)
		:name_(name),jointPosition_(jointPos),offsetPosition_(offsetPos),radius_(radius),
		defaultColor_(defaultColor), hitColor_(hitColor)
	{}

	//	ImGUi描画
	void DrawDebug();

	//	名前
	void SetName(const std::string& name) { name_ = name; }
	const std::string GetName()const { return name_; }

	//	基準にするジョイント位置
	void SetJointPosition(const DirectX::XMFLOAT3& jointPos) { jointPosition_ = jointPos; }
	const DirectX::XMFLOAT3 GetJointPosision()const { return jointPosition_; }

	//	ジョイント位置からのオフセット値
	void SetOffSetPosition(const DirectX::XMFLOAT3& offsetPos) { offsetPosition_ = offsetPos; }
	const DirectX::XMFLOAT3 GetOffsetPosition() const{ return offsetPosition_; }

	//	半径
	void SetRadius(const float& radius) { radius_ = radius; }
	const float GetRadius() { return radius_; }

	//	デフォルトカラー
	void SetDefaultColor(const DirectX::XMFLOAT4& color) { defaultColor_ = color; }
	const DirectX::XMFLOAT4 GetDefaultColor() { return defaultColor_; }

	//	ヒット時のカラー
	void SetHitColor(const DirectX::XMFLOAT4& color) { hitColor_ = color; }
	const DirectX::XMFLOAT4 GetHitColor() { return hitColor_; }

private:
	std::string			name_ = {};				//	名前
	DirectX::XMFLOAT3	jointPosition_ = {};	//	基準にするジョイントの位置
	DirectX::XMFLOAT3	offsetPosition_ = {};	//	ジョイント位置からのオフセット値
	float				radius_ = 0.0f;			//	半径
	DirectX::XMFLOAT4	defaultColor_ = {};		//	デフォルトカラー
	DirectX::XMFLOAT4	hitColor_ = {};			//	ヒット時のカラー

};

//	円柱判定用データ
struct CylinderData
{
public:
	CylinderData(const std::string& name,const DirectX::XMFLOAT3& jointPos,const DirectX::XMFLOAT3& offsetPos,
		const float& radius,const float& height,const DirectX::XMFLOAT4& defaultColor,const DirectX::XMFLOAT4& hitColor)
		:name_(name),jointPosition_(jointPos),offsetPosition_(offsetPos),radius_(radius),height_(height),
		defaultColor_(defaultColor),hitColor_(hitColor)
	{}

	//	ImGUi描画
	void DrawDebug();

	//	名前
	void SetName(const std::string& name) { name_ = name; }
	const std::string GetName()const { return name_; }

	//	基準にするジョイント位置
	void SetJointPosition(const DirectX::XMFLOAT3& jointPos) { jointPosition_ = jointPos; }
	const DirectX::XMFLOAT3 GetJointPosision()const { return jointPosition_; }

	//	ジョイント位置からのオフセット値
	void SetOffSetPosition(const DirectX::XMFLOAT3& offsetPos) { offsetPosition_ = offsetPos; }
	const DirectX::XMFLOAT3 GetOffsetPosition() const { return offsetPosition_; }

	//	半径
	void SetRadius(const float& radius) { radius_ = radius; }
	const float GetRadius() const{ return radius_; }

	//	高さ
	void SetHeight(const float& height) { height_ = height; }
	const float GetHeight() const { return height_; }

	//	デフォルトカラー
	void SetDefaultColor(const DirectX::XMFLOAT4& color) { defaultColor_ = color; }
	const DirectX::XMFLOAT4 GetDefaultColor() { return defaultColor_; }

	//	ヒット時のカラー
	void SetHitColor(const DirectX::XMFLOAT4& color) { hitColor_ = color; }
	const DirectX::XMFLOAT4 GetHitColor() { return hitColor_; }

private:
	std::string			name_ = {};				//	名前
	DirectX::XMFLOAT3	jointPosition_ = {};	//	基準にするジョイントの位置
	DirectX::XMFLOAT3	offsetPosition_ = {};	//	ジョイント位置からのオフセット値
	float				radius_ = 0.0f;			//	半径
	float				height_ = 0.0f;			//	高さ
	DirectX::XMFLOAT4	defaultColor_ = {};		//	デフォルトカラー
	DirectX::XMFLOAT4	hitColor_ = {};			//	ヒット時のカラー

};

//	攻撃判定
struct AttackDetectionData
{
public:
	AttackDetectionData(const std::string& name, const DirectX::XMFLOAT3& jointPos, const DirectX::XMFLOAT3& offsetPos,
		const float& radius, const DirectX::XMFLOAT4& defaultColor, const DirectX::XMFLOAT4& hitColor)
		:sphereData_({ name,jointPos,offsetPos,radius,defaultColor,hitColor })
	{}

private:
	SphereData sphereData_;	//	球体データ

};

//	くらい判定
struct DamageDetectionData
{
public:
	DamageDetectionData(const std::string& name, const DirectX::XMFLOAT3& jointPos, const DirectX::XMFLOAT3& offsetPos,
		const float& radius, const DirectX::XMFLOAT4& defaultColor, const DirectX::XMFLOAT4& hitColor)
		:sphereData_({ name,jointPos,offsetPos,radius,defaultColor,hitColor })
	{}

private:
	SphereData sphereData_;	//	球体データ

	float damage_ = 0.0f;	//	ダメージ量
	bool  isHit_ = false;	//	当たっているか
	float hitTimer_ = 0.0f;

};