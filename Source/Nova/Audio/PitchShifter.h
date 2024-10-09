#pragma once

#include "Frequency.h"

const int SAMPLE_RATE	= 44100;
const int FRAME_SIZE	= 2048;
const int OVERLAP		= FRAME_SIZE / 2;

//	�s�b�`�V�t�^�[
class PitchShifter
{
public:
	PitchShifter();
	~PitchShifter() {}

	void ApplyPitchShift(float pitchShift, const std::vector<float>& input, std::vector<float>& output);	//	�s�b�`�V�t�g�K��

private:
	Frequency* frequency_ = nullptr;

};

