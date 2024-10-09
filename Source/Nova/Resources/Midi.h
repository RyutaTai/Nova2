#pragma once

#include <iostream>
#include <vector>

//  MIDIトラッククラス
class MidiTrack 
{
public:
    enum class EventType    //  MIDIイベントの種類
    {
        NOTE_ON,
        NOTE_OFF
    };

    struct MidiNote         //  MIDIノートを表す構造体
    {
        EventType   eventType_;
        int         noteNumber_;
        double      time_;
    };

public:
    MidiTrack() {}
    ~MidiTrack() {}

    //  ノートを追加するメソッド
    void AddNote(EventType eventType, int noteNumber, double time);

    //  ノートを取得するメソッド
    std::vector<MidiNote> GetNotes() const { return notes_; }

private:
    std::vector<MidiNote> notes_;

};

//  リズム判定関数
void PerformRhythmAnalysis(const std::vector<MidiTrack::MidiNote>& notes)
{
    //  リズム判定の実装（ここでは単純にノートの時間を出力するだけ）
    for (const auto& note : notes) {
        std::cout << "Note Number: " << note.noteNumber_ << ", ";
        std::cout << "Event Type: " << (note.eventType_ == MidiTrack::EventType::NOTE_ON ? "Note On" : "Note Off") << ", ";
        std::cout << "Time: "       << note.time_ << " seconds" << std::endl;
    }
}

//  メイン関数
int main() {
    //  MIDIトラックを作成
    MidiTrack track;

    //  仮のノートを追加（実際のMIDI解析はここでは行わない）
    track.AddNote(MidiTrack::EventType::NOTE_ON, 60, 0.0);
    track.AddNote(MidiTrack::EventType::NOTE_OFF, 60, 1.0);

    //  リズム判定を実行
    PerformRhythmAnalysis(track.GetNotes());

    return 0;
}


