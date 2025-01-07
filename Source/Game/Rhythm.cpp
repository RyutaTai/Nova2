#include "Rhythm.h"

void Rhythm::Initialize()
{
	//	midiê∂ê¨
	midi_ = std::make_unique<Midi>("./Resources/Audio/MIDI/fourOnTheFloor.mid", 8.0f);
}

void Rhythm::Update(const float& elapsedTime)
{
	//	midiçXêVèàóù
	midi_->Update(elapsedTime);
}

void Rhythm::DrawDebug()
{

}