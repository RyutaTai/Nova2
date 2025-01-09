#include "Rhythm.h"

#include "UI/UIManager.h"
#include "UI/UIRhythmJudgment.h"
#include "../Nova/Core/Framework.h"
#include "../../imgui/imgui.h"

#include <algorithm>

void Rhythm::Initialize()
{
	//	midiの生成と初期化
	midi_ = std::make_unique<Midi>("./Resources/Audio/MIDI/fourOnTheFloor_140bpm.mid");
	//midi_ = std::make_unique<Midi>("./Resources/Audio/MIDI/fourOnTheFloor_140bpm_Loop.mid");
    midi_->Initialize();
}

void Rhythm::Update()
{
	//	midi更新処理
	midi_->Update(Framework::GetDoubleDeltaTime());

}

//  入力タイミングがリズムにあっているか判定する
void Rhythm::GetJudgmentType(const double& inputTime/*midiの範囲内でいつ入力されたか*/, const double& elapsedTime)
{
    //  ループ後の再生時間を考慮してノートを探索
    Midi::MidiNote* closestNote = midi_->FindClosestNoteInLoop(inputTime);

    //  ノートが見つからなければreturn
	if (closestNote == false)return;

    //  入力タイミングとのズレを計算
    double delta = std::abs(inputTime - closestNote->time_);
    debugDelta_ = delta;
    debugClosestNoteTime_ = closestNote->time_;
    debugInputTime_ = inputTime;

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
		float currentMidiTimer = static_cast<float>(midi_->GetCurrentTimer());
        ImGui::DragFloat("CurrentMidiTimer", &currentMidiTimer);
        //ImGui::DragFloat("ClosestNoteTime", &midi_->FindClosestNoteInLoop(currentMidiTimer)->time_);
		ImGui::DragFloat("PerfectRange", &PerfectRange_, 0.01f);
		ImGui::DragFloat("GoodRange", &GoodRange_, 0.01f);
		ImGui::DragFloat("Delta", &debugDelta_, 0.01f);
		ImGui::DragFloat("ClosestNoteTime", &debugClosestNoteTime_, 0.01f);
		ImGui::DragFloat("InputTime", &debugInputTime_, 0.01f);
        /* float inputTime = debugInputTime_;
		ImGui::DragFloat("InputTime", &inputTime, 0.01f);*/
        ImGui::TreePop();
    }
}