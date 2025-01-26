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
//void TimelineEditor::DrawTimeline() {
//    if (ImGui::Begin("Animation Editor")) {
//        ImGui::Text("Timeline Editor");
//        ImGui::Separator();
//
//        // 各イベントの描画
//        for (size_t i = 0; i < events_.size(); ++i) {
//            TimelineEvent& event = events_[i];
//            ImGui::PushID(static_cast<int>(i));
//
//            // イベントの個別UI描画
//            DrawEventUI(event, static_cast<int>(i));
//
//            ImGui::Separator();
//            ImGui::PopID();
//        }
//
//        // 新しいイベントを追加
//        if (ImGui::Button("Add New Event"))
//        {
//            AddEvent({ frameMin_, frameMin_ + 10, "New Event", {} });
//        }
//
//        //  JSON書き出し
//        if (ImGui::Button("SaveJSON"))
//        {
//            SaveToJson("./Resources/Json/");
//        }
//    }
//    ImGui::End();
//}

// タイムラインの描画
void TimelineEditor::DrawTimeline() 
{
    if (ImGui::Begin("Timeline Editor"))
    {
        ImGui::Text("Timeline Editor");
        ImGui::Separator();

        // 各イベントのタイムラインバーを描画
        for (size_t i = 0; i < events_.size(); ++i)
        {
            TimelineEvent& event = events_[i];
            DrawEventBar(event, static_cast<int>(i));
        }

        // 新しいイベントを追加
        if (ImGui::Button("Add New Event")) 
        {
            AddEvent({ frameMin_, frameMin_ + 10, "New Event", {}, GenerateColor(static_cast<int>(events_.size())) });
        }
    }
    ImGui::End();
}
// イベントバーの描画
void TimelineEditor::DrawEventBar(const TimelineEvent& event, int index) {
    float timelineWidth = ImGui::GetContentRegionAvail().x;
    float frameToPixels = timelineWidth / (frameMax_ - frameMin_); // フレームをピクセルに変換

    float barStart = frameToPixels * (event.startFrame_ - frameMin_);
    float barEnd = frameToPixels * (event.endFrame_ - frameMin_);

    // DirectX::XMFLOAT4をImGuiカラー形式 (unsigned int) に変換
    ImU32 color = IM_COL32(
        static_cast<int>(event.color_.x * 255.0f), // R
        static_cast<int>(event.color_.y * 255.0f), // G
        static_cast<int>(event.color_.z * 255.0f), // B
        static_cast<int>(event.color_.w * 255.0f)  // A
    );

    // 色付きの長方形を描画
    ImGui::GetWindowDrawList()->AddRectFilled(
        ImVec2(ImGui::GetCursorScreenPos().x + barStart, ImGui::GetCursorScreenPos().y),
        ImVec2(ImGui::GetCursorScreenPos().x + barEnd, ImGui::GetCursorScreenPos().y + 20),
        color
    );

    // イベント名の表示
    ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + barStart, ImGui::GetCursorScreenPos().y));
    ImGui::Text("%s", event.name_.c_str());

    ImGui::Dummy(ImVec2(0, 25)); // 次の行との間隔
}

// フレームごとの判定取得
std::vector<Hitbox> TimelineEditor::GetHitboxesAtFrame(const int& frame) const {
    std::vector<Hitbox> activeHitboxes;
    for (const auto& event : events_) {
        if (frame >= event.startFrame_ && frame <= event.endFrame_) {
            activeHitboxes.insert(activeHitboxes.end(), event.hitboxes_.begin(), event.hitboxes_.end());
        }
    }
    return activeHitboxes;
}

// JSON保存
void TimelineEditor::SaveToJson(const std::string& filepath) const {
    nlohmann::json json;
    for (const auto& event : events_) {
        nlohmann::json eventJson;
        eventJson["name"] = event.name_;
        eventJson["startFrame"] = event.startFrame_;
        eventJson["endFrame"] = event.endFrame_;
		eventJson["color"] = { event.color_.x, event.color_.y, event.color_.z, event.color_.w };
        for (const auto& hitbox : event.hitboxes_) {
            eventJson["hitboxes"].push_back({
                {"position", {hitbox.position_.x, hitbox.position_.y, hitbox.position_.z}},
                {"size", {hitbox.size_.x, hitbox.size_.y, hitbox.size_.z}},
                {"boneName", hitbox.boneName_}
                });
        }
        json.push_back(eventJson);
    }
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::filesystem::create_directories(std::filesystem::path(filepath).parent_path());
        file.open(filepath, std::ios::out);
    }
    if (file.is_open()) {
        file << json.dump(4);
    }
}

// JSON読み込み
void TimelineEditor::LoadFromJson(const std::string& filepath) {
    std::ifstream file(filepath);
    if (file.is_open()) {
        nlohmann::json json;
        file >> json;
        events_.clear();
        for (const auto& eventJson : json) {
            TimelineEvent event;
            event.name_ = eventJson["name"];
            event.startFrame_ = eventJson["startFrame"];
            event.endFrame_ = eventJson["endFrame"];
            event.color_ = 
            {    
                eventJson["color"][0],
                eventJson["color"][1],
                eventJson["color"][2],
                eventJson["color"][3]
			};
            for (const auto& hitboxJson : eventJson["hitboxes"]) {
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
                hitbox.boneName_ = hitboxJson["boneName"];
                event.hitboxes_.push_back(hitbox);
            }
            events_.push_back(event);
        }
    }
}

// 色を生成 (イベントごとに異なる色を割り当てる)
DirectX::XMFLOAT4 TimelineEditor::GenerateColor(int seed) const {
    srand(seed);
    return {
        static_cast<float>(rand() % 256) / 255.0f, // R
        static_cast<float>(rand() % 256) / 255.0f, // G
        static_cast<float>(rand() % 256) / 255.0f, // B
        1.0f // A (不透明)
    };
}
