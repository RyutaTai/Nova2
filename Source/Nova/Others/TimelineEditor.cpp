#include "TimelineEditor.h"

#include <fstream>
#include <d3d11.h>

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

// グリッドの描画
void TimelineEditor::DrawGrid() {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 start = ImGui::GetCursorScreenPos();
    float width = ImGui::GetContentRegionAvail().x;
    float height = 200; // グリッドエリアの高さ

    // フレーム間隔を設定
    const int gridSpacing = 10; // 10フレームごとにラインを描画
    float frameToPixels = width / (frameMax_ - frameMin_);

    // グリッドラインを描画
    for (int frame = frameMin_; frame <= frameMax_; frame += gridSpacing) {
        float x = start.x + (frame - frameMin_) * frameToPixels;
        drawList->AddLine(ImVec2(x, start.y), ImVec2(x, start.y + height), IM_COL32(200, 200, 200, 255));
    }

    // ベースラインを描画
    drawList->AddRect(start, ImVec2(start.x + width, start.y + height), IM_COL32(255, 255, 255, 255));
    ImGui::Dummy(ImVec2(0, height)); // レイアウトを確保
}

// タイムラインの描画
void TimelineEditor::DrawTimeline() {
    if (ImGui::Begin("Animation Editor")) {
        // グリッドを描画
        DrawGrid();

        // 各イベントの描画
        for (size_t i = 0; i < events_.size(); ++i) {
            DrawEventBar(events_[i], static_cast<int>(i));
        }

        // イベントの選択
        if (ImGui::Button("Add New Event")) {
            AddEvent({ frameMin_, frameMin_ + 10, "New Event", {}, {0.5f, 0.5f, 1.0f, 1.0f} });
        }
    }
    ImGui::End();
}

// イベントバーの描画
void TimelineEditor::DrawEventBar(TimelineEvent& event, const int& index) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 start = ImGui::GetCursorScreenPos();
    float timelineWidth = ImGui::GetContentRegionAvail().x;
    float frameToPixels = timelineWidth / (frameMax_ - frameMin_);

    // バーの位置
    float barStart = frameToPixels * (event.startFrame_ - frameMin_);
    float barEnd = frameToPixels * (event.endFrame_ - frameMin_);
    float barHeight = 20.0f;

    ImVec2 barPosStart = ImVec2(start.x + barStart, start.y);
    ImVec2 barPosEnd = ImVec2(start.x + barEnd, start.y + barHeight);

    // バーの描画
    drawList->AddRectFilled(barPosStart, barPosEnd, ImGuiColor(event.color_), 4.0f);
    drawList->AddRect(barPosStart, barPosEnd, IM_COL32(255, 255, 255, 255), 4.0f);

    // ドラッグ操作でフレーム調整
    if (ImGui::IsMouseHoveringRect(barPosStart, barPosEnd) && ImGui::IsMouseDown(0)) {
        ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
        int frameDelta = static_cast<int>(mouseDelta.x / frameToPixels);
        event.startFrame_ += frameDelta;
        event.endFrame_ += frameDelta;
        event.startFrame_ = max(event.startFrame_, frameMin_);
        event.endFrame_ = min(event.endFrame_, frameMax_);
    }

    // 名前表示
    ImGui::SetCursorScreenPos(barPosStart);
    ImGui::Text("%s", event.name_.c_str());
    ImGui::Dummy(ImVec2(0, barHeight + 5.0f)); // レイアウト用
}

// ImGuiカラー変換
unsigned int TimelineEditor::ImGuiColor(const DirectX::XMFLOAT4& color) const {
    return IM_COL32(
        static_cast<int>(color.x * 255.0f),
        static_cast<int>(color.y * 255.0f),
        static_cast<int>(color.z * 255.0f),
        static_cast<int>(color.w * 255.0f)
    );
}
