#include "Midi.h"

//  �m�[�g��ǉ����郁�\�b�h
void MidiTrack::AddNote(EventType eventType, int noteNumber, double time)
{
    MidiNote note;
    note.eventType_     = eventType;
    note.noteNumber_    = noteNumber;
    note.time_          = time;
    notes_.emplace_back(note);

}
