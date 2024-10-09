#pragma once

#include <d3d11.h>
#include <memory>
#include <thread>
#include <complex>
#include <vector>

using Complex = std::complex<double>;

//	�t�[���G�ϊ�
class Frequency
{
public:
	Frequency();
	~Frequency() {}

	void FFT(std::vector<Complex>& x);				//	�t�[���G�ϊ�

	std::vector<float> HammingWindow(int count);	//	�n�~���O��

private:
	static constexpr float AUDIO_PI			= 3.14159265358979323846f;
	static constexpr float AUDIO_PI_LONG	= 3.14159265358979323846264338328L;

};

