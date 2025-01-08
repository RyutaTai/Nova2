#include "Rhythm.h"

#include "UI/UIManager.h"
#include "../../imgui/imgui.h"

void Rhythm::Initialize()
{
	//	midi生成
	midi_ = std::make_unique<Midi>("./Resources/Audio/MIDI/fourOnTheFloor2.mid");
}

void Rhythm::Update(const float& elapsedTime)
{
	//	midi更新処理
	midi_->Update(elapsedTime);

}

void Rhythm::GetJudgmentType(const float& inputTime/*midiの範囲内でいつ入力されたか*/, const float& elapsedTime)
{
    //  ループ後の再生時間を考慮してノートを探索
    Midi::MidiNote* closestNote = midi_->FindClosestNoteInLoop(inputTime);

    //  ノートが見つからなければreturn
	if (closestNote == false)return;

    //  入力タイミングとのズレを計算
    float delta = std::abs(inputTime - closestNote->time_);

    //  判定済みノートは無視
	if (closestNote->judged_) return;

    //  判定範囲による判定
    if (delta <= PerfectRange_)
    {
        closestNote->judged_ = true;
        UI* uiRhythm = UIManager::Instance().GetUIFromType(UIManager::UIType::RhythmJudgment);
        uiRhythm->SetIsVisible(true);
        //return JudgmentType::Perfect;
    }
    else if (delta <= GoodRange_)
    {
        closestNote->judged_ = true;
        UI* uiRhythm = UIManager::Instance().GetUIFromType(UIManager::UIType::RhythmJudgment);
        uiRhythm->SetIsVisible(true);
        //return JudgmentType::Good;
    }
    else
    {
        UI* uiRhythm = UIManager::Instance().GetUIFromType(UIManager::UIType::RhythmJudgment);
        uiRhythm->SetIsVisible(true);
        //return JudgmentType::Miss;
    }
}

void Rhythm::DrawDebug()
{
    if(ImGui::TreeNode("Rhythm"))
    {
        float currentMidiTimer = midi_->GetCurrentTimer();
        ImGui::DragFloat("CurrentMidiTimer", &currentMidiTimer);
        ImGui::DragFloat("ClosestNoteTime", &midi_->FindClosestNoteInLoop(currentMidiTimer)->time_);
        ImGui::TreePop();
    }
}