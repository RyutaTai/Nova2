#include "TimelineEditor.h"

#include <fstream>

#include "../../nlohmann/json.hpp"

//  イベント追加
void TimelineEditor::AddEvent(const TimelineEvent& event)
{
    events_.push_back(event);
}

//  イベント削除
void TimelineEditor::RemoveEvent(const int& index) 
{
    if (index >= 0 && index < events_.size()) 
    {
        events_.erase(events_.begin() + index);
    }
}

//  イベント更新
void TimelineEditor::UpdateEvent(const int& index, const TimelineEvent& event) 
{
    if (index >= 0 && index < events_.size()) 
    {
        events_[index] = event;
    }
}

////  タイムラインUI描画
void TimelineEditor::DrawUI() 
{
    if (ImGui::Begin("Timeline Editor")) 
    {
        for (size_t i = 0; i < events_.size(); ++i)
        {
            TimelineEvent& event = events_[i];
            ImGui::PushID(static_cast<int>(i));

            //  イベント名とフレーム範囲
            ImGui::Text("Event: %s", event.name_.c_str());
            ImGui::SliderInt("Start Frame", &event.startFrame_, frameMin_, frameMax_);
            ImGui::SliderInt("End Frame", &event.endFrame_, frameMin_, frameMax_);

            //  判定データの編集
			for (size_t j = 0; j < event.hitboxes_.size(); ++j)
            {
                Hitbox& hitbox = event.hitboxes_[j];
                ImGui::Text("Hitbox %zu", j);
                ImGui::DragFloat3("Position", &hitbox.position_.x, 0.1f);
                ImGui::DragFloat3("Size", &hitbox.size_.x, 0.1f);
                ImGui::InputInt("Bone Index", &hitbox.boneIndex_);
            }

            if (ImGui::Button("Delete Event")) 
            {
                RemoveEvent(static_cast<int>(i));
            }
            ImGui::PopID();
        }

        if (ImGui::Button("Add New Event")) 
        {
            AddEvent({ 0, 0, "New Event", {} });
        }
    }
    ImGui::End();
}

// タイムラインの描画
void TimelineEditor::DrawTimeline() {
    if (ImGui::Begin("Animation Editor")) {
        ImGui::Text("Timeline Editor");
        ImGui::Separator();

        // 各イベントの描画
        for (size_t i = 0; i < events_.size(); ++i) {
            TimelineEvent& event = events_[i];
            ImGui::PushID(static_cast<int>(i));

            // イベントの個別UI描画
            DrawEventUI(event, static_cast<int>(i));

            ImGui::Separator();
            ImGui::PopID();
        }

        // 新しいイベントを追加
        if (ImGui::Button("Add New Event"))
        {
            AddEvent({ frameMin_, frameMin_ + 10, "New Event", {} });
        }

        //  JSON書き出し
        if (ImGui::Button("SaveJSON"))
        {
            SaveToJson("./Resources/Json/");
        }
    }
    ImGui::End();
}

// イベントの個別UI
void TimelineEditor::DrawEventUI(TimelineEvent& event, const int& index) {
    ImGui::Text("Event %d: %s", index, event.name_.c_str());
    ImGui::InputText("Name", &event.name_[0], 64);
    ImGui::SliderInt("Start Frame", &event.startFrame_, frameMin_, frameMax_);
    ImGui::SliderInt("End Frame", &event.endFrame_, frameMin_, frameMax_);

    // 判定データの編集
    for (size_t j = 0; j < event.hitboxes_.size(); ++j) 
    {
        Hitbox& hitbox = event.hitboxes_[j];
        ImGui::Text("Hitbox %zu", j);
        ImGui::PushID(static_cast<int>(j));
        ImGui::DragFloat3("Position", &hitbox.position_.x, 0.1f);
        ImGui::DragFloat3("Size", &hitbox.size_.x, 0.1f);
        ImGui::InputInt("Bone Index", &hitbox.boneIndex_);
        ImGui::PopID();
    }

    // 判定を追加
    if (ImGui::Button("Add Hitbox")) {
        event.hitboxes_.push_back({ {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, -1 });
    }

    // イベントを削除
    if (ImGui::Button("Delete Event")) {
        RemoveEvent(index);
    }
}

//  指定フレームの判定を取得
std::vector<Hitbox> TimelineEditor::GetHitboxesAtFrame(const int& frame) const 
{
    std::vector<Hitbox> activeHitboxes;
    for (const auto& event : events_)
    {
        if (frame >= event.startFrame_ && frame <= event.endFrame_) 
        {
            activeHitboxes.insert(activeHitboxes.end(), event.hitboxes_.begin(), event.hitboxes_.end());
        }
    }
    return activeHitboxes;
}

//  JSON保存
void TimelineEditor::SaveToJson(const std::string& filepath) const {
    nlohmann::json json;
    for (const auto& event : events_) 
    {
        nlohmann::json eventJson;
        eventJson["name"]       = event.name_;
        eventJson["startFrame"] = event.startFrame_;
        eventJson["endFrame"]   = event.endFrame_;
        for (const auto& hitbox : event.hitboxes_) 
        {
            eventJson["hitboxes"].push_back({
				{"position", {hitbox.position_.x, hitbox.position_.y, hitbox.position_.z}},
                {"size", {hitbox.size_.x, hitbox.size_.y, hitbox.size_.z}},
                {"boneIndex", hitbox.boneIndex_}
                });
        }
        json.push_back(eventJson);
    }
    std::ofstream file(filepath);
    if (file.is_open()) 
    {
        file << json.dump(4);
    }
}

//  JSON読み込み
void TimelineEditor::LoadFromJson(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (file.is_open()) 
    {
        nlohmann::json json;
        file >> json;
        events_.clear();
        for (const auto& eventJson : json) 
        {
            TimelineEvent event;
            event.name_         = eventJson["name"];
            event.startFrame_   = eventJson["startFrame"];
            event.endFrame_     = eventJson["endFrame"];
            for (const auto& hitboxJson : eventJson["hitboxes"]) 
            {
                Hitbox hitbox;
                hitbox.position_ = {
                    hitboxJson["position"][0],
                    hitboxJson["position"][1],
                    hitboxJson["position"][2]
                };
                hitbox.size_ = {
                    hitboxJson["size"][0],
                    hitboxJson["size"][1],
                    hitboxJson["size"][2]
                };
                hitbox.boneIndex_ = hitboxJson["boneIndex"];
                event.hitboxes_.push_back(hitbox);
            }
            events_.push_back(event);
        }
    }
}

