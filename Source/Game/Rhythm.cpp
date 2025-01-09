#include "Rhythm.h"

#include "UI/UIManager.h"
#include "UI/UIRhythmJudgment.h"
#include "../../imgui/imgui.h"

void Rhythm::Initialize()
{
	//	midiの生成と初期化
	//midi_ = std::make_unique<Midi>("./Resources/Audio/MIDI/fourOnTheFloor_140bpm.mid");
	midi_ = std::make_unique<Midi>("./Resources/Audio/MIDI/fourOnTheFloor_140bpm_Loop.mid");
    midi_->Initialize();
}

void Rhythm::Update(const float& elapsedTime)
{
	//	midi更新処理
	midi_->Update(elapsedTime);

}

//  入力タイミングがリズムにあっているか判定する
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
        //  判定文字UIを生成
        UIRhythmJudgment* uiRhythm = new UIRhythmJudgment(JudgmentType::Perfect);
        uiRhythm->Initialize();
        uiRhythm->SetIsVisible(true);
    }
    else if (delta <= GoodRange_)
    {
        closestNote->judged_ = true;
        //  判定文字UIを生成
        UIRhythmJudgment* uiRhythm = new UIRhythmJudgment(JudgmentType::Good);
        uiRhythm->Initialize();
        uiRhythm->SetIsVisible(true);
    }
    else
    {
        //  判定文字UIを生成
        UIRhythmJudgment* uiRhythm = new UIRhythmJudgment(JudgmentType::Miss);
        uiRhythm->Initialize();
        uiRhythm->SetIsVisible(true);
    }
}

void Rhythm::DrawDebug()
{
    if(ImGui::TreeNode("Rhythm"))
    {
        float currentMidiTimer = midi_->GetCurrentTimer();
        ImGui::DragFloat("CurrentMidiTimer", &currentMidiTimer);
        ImGui::DragFloat("ClosestNoteTime", &midi_->FindClosestNoteInLoop(currentMidiTimer)->time_);
        ImGui::DragFloat("PerfectRange", &PerfectRange_);
        ImGui::DragFloat("GoodRange", &GoodRange_);
        ImGui::TreePop();
    }
}