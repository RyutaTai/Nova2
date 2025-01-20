#include "CollisionData.h"

#include "../../imgui/imgui.h"

//	球判定用ImGui
void CollisionSphereData::DrawDebug()
{
	if (ImGui::TreeNode(name_.c_str()))
	{
		ImGui::DragFloat3("JointPos", &jointPosition_.x, 0.01f);
		ImGui::DragFloat3("OffsetPos", &offsetPosition_.x, 0.01f);
		ImGui::DragFloat("Radius", &radius_, 0.01f);
		ImGui::ColorEdit4("Color", &defaultColor_.x);

		ImGui::TreePop();
	}
}

//	円柱判定用ImGui
void CollisionCylinderData::DrawDebug()
{
	if (ImGui::TreeNode(name_.c_str()))
	{
		ImGui::DragFloat3("JointPos", &jointPosition_.x, 0.01f);
		ImGui::DragFloat3("OffsetPos", &offsetPosition_.x, 0.01f);
		ImGui::DragFloat("Radius", &radius_, 0.01f);
		ImGui::ColorEdit4("Color", &defaultColor_.x);

		ImGui::TreePop();
	}
}

//	攻撃判定用
void AttackDetectionData::DrawDebug()
{
	collisionSphereData_.DrawDebug();
}

//	くらい判定更新処理
void DamageDetectionData::Update(const float& elapsedTime)
{
	//	ヒットしたら色を変える
	if (isHit_)
	{
		hitTimer_ -= elapsedTime;

		if (hitTimer_ < 0.0f)
		{
			isHit_ = false;
		}
		SetColor(GetHitColor());
	}
	else
	{
		SetColor(GetDefaultColor());
	}
}

//	くらい判定用
void DamageDetectionData::DrawDebug()
{
	collisionSphereData_.DrawDebug();
}

//	押し出し判定用
void CollisionDetectionData::DrawDebug()
{
	collisionSphereData_.DrawDebug();
}
