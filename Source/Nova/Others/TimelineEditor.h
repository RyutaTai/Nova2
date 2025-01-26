#pragma once

#include <vector>

#include "../../imgui/imgui.h"
#include "../../ImGuizmo-master/ImSequencer.h"  // Sequencer用のヘッダーをインクルード

#pragma once
#include <string>
#include <vector>
#include <functional>
#include <DirectXMath.h>

//  判定データ
struct Hitbox 
{
    DirectX::XMFLOAT3   position_   = {};                   //  判定の位置
	DirectX::XMFLOAT3   size_       = { 1.0f,1.0f,1.0f };   //  判定の大きさ
	int                 boneIndex_  = -1;                   //  ボーン番号 (-1の場合、ボーンを使用しない)
    std::string boneName_ = {};
};

//  イベントデータ (アニメーション判定用)
struct TimelineEvent 
{
	int                 startFrame_ = -1;    //  開始フレーム
	int                 endFrame_   = -1;    //  終了フレーム
    std::string         name_       = {};    //  イベント名 (例: "攻撃判定1")
    std::vector<Hitbox> hitboxes_;           //  判定データのリスト
    DirectX::XMFLOAT4   color_ = {0.0f,0.0f,0.0f,1.0f};
};

//  タイムラインエディタクラス
class TimelineEditor 
{
public:
    TimelineEditor(const int& frameMin, const int& frameMax) 
        : frameMin_(frameMin), frameMax_(frameMax) 
    {}

    TimelineEditor() = default;

    //  タイムライン操作
    void AddEvent(const TimelineEvent& event);
    void RemoveEvent(const int& index);
    void UpdateEvent(const int& index, const TimelineEvent& event);

    //  描画 (ImGuiでタイムラインを表示)
    void DrawGrid();

    void DrawTimeline();
	void DrawEventUI(TimelineEvent& event, const int& index);

    void DrawEventBar(TimelineEvent& event, const int& index);
    unsigned int ImGuiColor(const DirectX::XMFLOAT4& color) const;

    //  フレームごとの判定取得
    std::vector<Hitbox> GetHitboxesAtFrame(const int& frame) const;

    // データ保存・読み込み
    void SaveToJson(const std::string& filepath) const;
    void LoadFromJson(const std::string& filepath);

private:
	int frameMin_ = 0;
	int frameMax_ = 0;
    std::vector<TimelineEvent> events_;

};


/// <summary>
/// /////////////////////////////////////////////////////////////////////////////////////////////////////
/// </summary>
//
////  イベントデータ用のクラスを継承して定義
//struct MySequence : public ImSequencer::SequenceInterface 
//{
//    struct Event 
//    {
//        int startFrame_ = 0;
//		int endFrame_ = 0;
//        int type_ = -1;  // 0: Animation, 1: Sound
//        unsigned int color_ = 0;
//    };
//
//    std::vector<Event> events_;
//
//    //  タイムラインの最小フレームと最大フレームを取得
//    virtual int GetFrameMin() const override { return 0; }
//    virtual int GetFrameMax() const override { return 100; }
//
//    //  イベントの数を取得
//    virtual int GetItemCount() const override { return static_cast<int>(events_.size()); }
//
//    // イベントのデータを取得
//    virtual void Get(int index, int** start, int** end, int* type, unsigned int* color) override
//    {
//        Event& evt  = events_[index];
//        *start      = &evt.startFrame_;
//        *end        = &evt.endFrame_;
//        *type       = evt.type_;
//        *color      = evt.color_;
//    }
//
//    //  イベントのラベルを取得
//    virtual const char* GetItemLabel(int index) const override
//    {
//        if (events_[index].type_ == 0) return "Animation";
//        if (events_[index].type_ == 1) return "Sound";
//        return "Unknown";
//    }
//
//    //  イベントの追加
//    virtual void Add(int type) override 
//    {
//        events_.push_back({ 10, 20, type, (type == 0) ? 0xFF00FF00 : 0xFF0000FF });
//    }
//
//    //  イベントの削除
//    virtual void Del(int index) override 
//    {
//        if (index >= 0 && index < static_cast<int>(events_.size()))
//        {
//            events_.erase(events_.begin() + index);
//        }
//    }
//
//    //  イベントの複製
//    virtual void Duplicate(int index) override
//    {
//        if (index >= 0 && index < static_cast<int>(events_.size())) {
//            Event evt = events_[index];
//            events_.push_back(evt);
//        }
//    }
//
//    //  コンストラクタで初期データを追加（任意）
//    MySequence() 
//    {
//        Add(0); // Animationイベントを追加
//        Add(1); // Soundイベントを追加
//    }
//};
//
////  グローバル変数としてMySequenceのインスタンスを作成
//MySequence mySequence;
//
////  タイムラインUIを描画する関数
//void DrawTimelineEditor()
//{
//    static int currentFrame = 0;
//    static int selectedEntry = -1;
//    static bool expanded = true;
//    static int firstFrame = 0;
//
//    //  Sequencerの呼び出し
//    ImSequencer::Sequencer(
//        &mySequence,                    //  シーケンスデータ
//        &currentFrame,                  //  現在のフレーム
//        &expanded,                      //  展開状態
//        &selectedEntry,                 //  選択中のエントリ
//        &firstFrame,                    //  表示する最初のフレーム
//        ImSequencer::SEQUENCER_EDIT_ALL //  編集オプション
//    );
//}
//
//void RenderUI() {
//    ImGui::Begin("Timeline Editor");
//    DrawTimelineEditor();
//    ImGui::End();
//}