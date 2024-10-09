#pragma once

#include <iostream>
#include <vector>

//  MIDI�g���b�N�N���X
class MidiTrack 
{
public:
    enum class EventType    //  MIDI�C�x���g�̎��
    {
        NOTE_ON,
        NOTE_OFF
    };

    struct MidiNote         //  MIDI�m�[�g��\���\����
    {
        EventType   eventType_;
        int         noteNumber_;
        double      time_;
    };

public:
    MidiTrack() {}
    ~MidiTrack() {}

    //  �m�[�g��ǉ����郁�\�b�h
    void AddNote(EventType eventType, int noteNumber, double time);

    //  �m�[�g���擾���郁�\�b�h
    std::vector<MidiNote> GetNotes() const { return notes_; }

private:
    std::vector<MidiNote> notes_;

};

//  ���Y������֐�
void PerformRhythmAnalysis(const std::vector<MidiTrack::MidiNote>& notes)
{
    //  ���Y������̎����i�����ł͒P���Ƀm�[�g�̎��Ԃ��o�͂��邾���j
    for (const auto& note : notes) {
        std::cout << "Note Number: " << note.noteNumber_ << ", ";
        std::cout << "Event Type: " << (note.eventType_ == MidiTrack::EventType::NOTE_ON ? "Note On" : "Note Off") << ", ";
        std::cout << "Time: "       << note.time_ << " seconds" << std::endl;
    }
}

//  ���C���֐�
int main() {
    //  MIDI�g���b�N���쐬
    MidiTrack track;

    //  ���̃m�[�g��ǉ��i���ۂ�MIDI��͂͂����ł͍s��Ȃ��j
    track.AddNote(MidiTrack::EventType::NOTE_ON, 60, 0.0);
    track.AddNote(MidiTrack::EventType::NOTE_OFF, 60, 1.0);

    //  ���Y����������s
    PerformRhythmAnalysis(track.GetNotes());

    return 0;
}


