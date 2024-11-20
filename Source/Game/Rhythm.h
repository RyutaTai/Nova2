#pragma once

class Rhythm
{
public:
	Rhythm() {}
	~Rhythm() {}

	static Rhythm& Instance()
	{
		static Rhythm rhythm;
		return rhythm;
	}

	void SetBPM(const int& bpm) { bpm_ = bpm; }

	int GetBPM() { return bpm_; }

private:
	int bpm_ = 120;

};

