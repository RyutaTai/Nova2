#include "Frequency.h"

//  ハミング窓
//  http://www.densikairo.com/Development/Public/study_dsp/C1EBB4D8BFF4.html
//  https://cognicull.com/ja/qc1y1tr9
std::vector<float> Frequency::HammingWindow(int count)
{
    std::vector<float> hm;
    for (int i = 0; i < count; ++i)
    {
        float h;
        h = 0.54f - (0.46f * cosf((2 * AUDIO_PI * i) / (count - 1)));
        hm.emplace_back(h);
    }
    return hm;
}

//  フーリエ変換
void Frequency::FFT(std::vector<Complex>& x)
{
    unsigned int N = x.size(), k = N, n;
    double thetaT = AUDIO_PI_LONG / N;

    // DFT
    Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;
    while (k > 1)
    {
        n = k;
        k >>= 1;
        phiT = phiT * phiT;
        T = 1.0L;
        for (unsigned int l = 0; l < k; l++)
        {
            for (unsigned int a = l; a < N; a += n)
            {
                unsigned int b = a + k;
                Complex t = x[a] - x[b];
                x[a] += x[b];
                x[b] = t * T;
            }
            T *= phiT;
        }
    }
    // Decimate
    unsigned int m = (unsigned int)log2(N);
    for (unsigned int a = 0; a < N; a++)
    {
        unsigned int b = a;
        // Reverse bits
        b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
        b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
        b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
        b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
        b = ((b >> 16) | (b << 16)) >> (32 - m);
        if (b > a)
        {
            Complex t = x[a];
            x[a] = x[b];
            x[b] = t;
        }
    }
}
