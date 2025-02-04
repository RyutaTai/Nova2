#include "Rhythm.h"

#include "UI/UIManager.h"
#include "UI/UIRhythmJudgment.h"
#include "../Nova/Core/Framework.h"
#include "../../imgui/imgui.h"

#include <algorithm>

void Rhythm::Initialize()
{
	//	midiの生成と初期化
	midi_ = std::make_unique<Midi>("./Resources/Audio/MIDI/fourOnTheFloor_140bpm.mid", 1.714);
	//midi_ = std::make_unique<Midi>("./Resources/Audio/MIDI/fourOnTheFloor_140bpm_Loop.mid");
    midi_->Initialize();
}

void Rhythm::Update()
{
	//	midi更新処理
	midi_->Update(Framework::GetDoubleDeltaTime());

}

//  入力タイミングがリズムにあっているか判定する
Rhythm::JudgmentType Rhythm::GetJudgmentType(const double& inputTime/*midiの範囲内でいつ入力されたか*/)
{
    //  ループ後の再生時間を考慮してノートを探索
    //Midi::MidiNote* closestNote = midi_->FindClosestNoteInLoop(inputTime);
    Midi::MidiNote* closestNote = midi_->FindClosestNote(inputTime);

    //  ノートが見つからなければreturn
	if (closestNote == false)return JudgmentType::None;

    //  入力タイミングとのズレを計算
	double deltaPlus = std::abs(closestNote->time_ - inputTime);    //  過去の一番近いノーツからプラス方向の差分
    double deltaMinus;  //  未来の一番近いノーツの中でマイナス方向の差分(未来のノートの位置 - delta)
    debugDelta_ = deltaPlus;
    debugClosestNoteTime_ = closestNote->time_;
    debugInputTime_ = inputTime;

    //  判定済みノートは無視
	if (closestNote->judged_) return JudgmentType::None;

    //  判定範囲による判定
    if (deltaPlus <= PerfectRange_)
    {
        closestNote->judged_ = true;

        //  判定文字UIを生成
        UIManager::Instance().RemoveFromType(UIManager::UIType::Rhythm);
        UIRhythmJudgment* uiRhythm = new UIRhythmJudgment(JudgmentType::Perfect);
        uiRhythm->Initialize();
        uiRhythm->SetIsVisible(true);

        return Rhythm::JudgmentType::Perfect;
    }
    else if (deltaPlus <= GoodRange_)
    {
        closestNote->judged_ = true;
        //  判定文字UIを生成
        UIManager::Instance().RemoveFromType(UIManager::UIType::Rhythm);
        UIRhythmJudgment* uiRhythm = new UIRhythmJudgment(JudgmentType::Good);
        uiRhythm->Initialize();
        uiRhythm->SetIsVisible(true);

        return Rhythm::JudgmentType::Good;
    }
    else
    {
        //  判定文字UIを生成
        UIManager::Instance().RemoveFromType(UIManager::UIType::Rhythm);
        UIRhythmJudgment* uiRhythm = new UIRhythmJudgment(JudgmentType::Miss);
        uiRhythm->Initialize();
        uiRhythm->SetIsVisible(true);

        return Rhythm::JudgmentType::Miss;
    }
}

//	midiを見てノートオンならtrueを返す(テンポに合わせた動きをさせるために使用する)
bool Rhythm::GetRhythm()
{
    //  現在ノートオンならtrueを返す
    if (midi_->IsInputNoteOn(GetCurrentMidiTime()))
        return true;

    return false;
}

void Rhythm::DrawDebug()
{
    if(ImGui::TreeNode("Rhythm"))
    {
		float currentMidiTimer = static_cast<float>(midi_->GetCurrentTimer());
		float maxMidiTimer = debugMaxMidiTimer_;
		debugMaxMidiTimer_ = max(debugMaxMidiTimer_, midi_->GetCurrentTimer());

        ImGui::DragFloat("CurrentMidiTimer", &currentMidiTimer);
        ImGui::DragFloat("MaxMidiTimer", &maxMidiTimer);
        //ImGui::DragFloat("ClosestNoteTime", &midi_->FindClosestNoteInLoop(currentMidiTimer)->time_);

        float midiDuration = midi_->GetMidiFileDurationSeconds();
        ImGui::DragFloat("MIdiDuration", &midiDuration);   //   midiファイルの長さ[s]

        //  判定範囲
        float perfectRange = PerfectRange_;
        float goodRange = GoodRange_;
		ImGui::DragFloat("PerfectRange", &perfectRange, 0.01f);
		ImGui::DragFloat("GoodRange", &goodRange, 0.01f);
        PerfectRange_ = perfectRange;
        GoodRange_ = goodRange;
		
        ImGui::DragFloat("Delta", &debugDelta_, 0.01f);
		ImGui::DragFloat("ClosestNoteTime", &debugClosestNoteTime_, 0.01f);
		ImGui::DragFloat("InputTime", &debugInputTime_, 0.01f);
        
        ImGui::TreePop();
    }
}