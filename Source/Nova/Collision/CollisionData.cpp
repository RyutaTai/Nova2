#include "CollisionData.h"

#include "../../imgui/imgui.h"

//	‹…”»’è—pImGui
void SphereData::DrawDebug()
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

//	‰~’Œ”»’è—pImGui
void CylinderData::DrawDebug()
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
