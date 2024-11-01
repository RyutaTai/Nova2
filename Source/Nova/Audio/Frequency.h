#pragma once

#include <d3d11.h>
#include <memory>
#include <thread>
#include <complex>
#include <vector>

#include "AudioSource.h"

using Complex = std::complex<double>;

//	フーリエ変換
class Frequency
{
public:
	Frequency() {}
	~Frequency() {}

	void Initialize();
	void Update(const float& elapsedTime,const std::shared_ptr<AudioSource>& audioSource);
	void DrawDebug();

	void FFT(std::vector<Complex>& x);				//	フーリエ変換

	std::vector<float> HammingWindow(const int& count);	//	ハミング窓

	std::vector<float> GetAmplitudeSpectrum() { return amplitudeSpectrum_; }
	float GetAmplitudeSpectrum(const int& index) { return amplitudeSpectrum_.at(index); }

private:
	static constexpr float AUDIO_PI			= 3.14159265358979323846f;
	static constexpr float AUDIO_PI_LONG	= 3.14159265358979323846264338328L;

	static constexpr int blockCount_ = 2048;		//	ハミング窓サンプル数(何分割するか)
	//static constexpr int blockCount_ = 1024;		//	ハミング窓サンプル数(何分割するか)

	std::vector<float> amplitudeSpectrum_;			//	振幅スぺクトラム(周波数帯ごとのデシベル値)
	std::vector<float> oldAmplitudeSpectrum_;		//	前回の振幅スペクトラム
	std::vector<float> hamming_;

	float audioTimer_ = 0.0f;	//	再生時間

};

