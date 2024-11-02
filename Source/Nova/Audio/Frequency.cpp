#include "Frequency.h"

#include "../../../External/imgui/imgui.h"
#include "../../Nova/Others/Converter.h"

void Frequency::Initialize()
{
    // Hamming窓の生成
    hamming_ = HammingWindow(blockCount_);

    oldAmplitudeSpectrum_.resize(blockCount_, 0.0f);

}

void Frequency::Update(const float& elapsedTime,const std::shared_ptr<AudioSource>& audioSource)
{
#if 1   //自分で変えた
    size_t          SPsize      = audioSource->GetAudioBytes();     //  オーディオのバッファサイズ取得
    const BYTE*     SPdata      = audioSource->GetAudioData();
    //std::vector<uint8_t> audioVector = ConvertToVector(SPdata, SPsize);
    const uint8_t* audioVector  = audioSource->GetAudioData();
    int             SPNowData   = audioSource->GetCurrentSample();  //  現在のサンプル
    int             SPNowBlock  = SPNowData / blockCount_;          //  現在のブロック計算

    //  FFT変換
    std::vector<Complex> windowedData;
    int spNowBlock = blockCount_ * SPNowBlock;

    for (int i = 0; i < blockCount_; ++i)
    {
        int index = i + spNowBlock;
        if (index < SPsize)  // 範囲内かチェック
        {
            windowedData.emplace_back(hamming_[i] * audioVector[index]);
        }
        else
        {
            windowedData.emplace_back(0.0f);    //  範囲外なら0を追加        }
        }
    }
    FFT(windowedData);

    //  振幅スペクトル
    amplitudeSpectrum_.clear();
    for (auto& w : windowedData)
    {
        amplitudeSpectrum_.emplace_back(sqrtf(w.real() * w.real() + w.imag() * w.imag()));
    }

    //  平滑化
    float blendRate = 0.9f;
    for (size_t i = 0; i < amplitudeSpectrum_.size() - 1; ++i)
    {
        amplitudeSpectrum_[i] = blendRate * oldAmplitudeSpectrum_[i] + ((1 - blendRate) * amplitudeSpectrum_[i]);
        oldAmplitudeSpectrum_[i] = amplitudeSpectrum_[i];
    }
#else
    UINT32 SPsize = audioSource->GetAudioBytes();
    //auto& SPdata = audioSource->GetAudioData();
    auto& SPdata = ConvertToVector(audioSource->GetAudioData(), SPsize);
    int SPNowData = audioSource->GetCurrentSample();    // 現在のサンプル
    int SPNowBlock = SPNowData / blockCount_;    // 現在のブロック計算

    // SPNowBlock が SPdata の範囲を超えないようにする
    if (SPNowBlock * blockCount_ + blockCount_ > SPsize)
    {
        SPNowBlock = (SPsize - blockCount_) / blockCount_;
    }

    // windowedDataのサイズをdataBlockSizeに設定する
    std::vector<Complex> windowedData(blockCount_);

    for (int i = 0; i < blockCount_; ++i)
    {
        // 範囲チェックを追加する
        if ((i + blockCount_ * SPNowBlock) < SPsize)
        {
            windowedData[i] = hamming_[i] * SPdata[i + blockCount_ * SPNowBlock];
        }
        else
        {
            windowedData[i] = 0; // 範囲外の場合は0を代入する
        }
    }

    FFT(windowedData);

    // 振幅スペクトル
    amplitudeSpectrum_.clear();
    for (auto& w : windowedData)
    {
        amplitudeSpectrum_.emplace_back(sqrtf(w.real() * w.real() + w.imag() * w.imag()));
    }

    // 平滑化
    for (size_t i = 0; i < amplitudeSpectrum_.size(); ++i)
    {
        if (i < oldAmplitudeSpectrum_.size())
        {
            amplitudeSpectrum_[i] = 0.97f * oldAmplitudeSpectrum_[i] + (0.03f * amplitudeSpectrum_[i]);
        }
        else
        {
            amplitudeSpectrum_[i] = amplitudeSpectrum_[i]; // もし oldAmplitudeSpectrum に対応するインデックスがない場合はそのまま
        }
    }

    // oldAmplitudeSpectrumのサイズを更新する
    oldAmplitudeSpectrum_ = amplitudeSpectrum_;

    //sec = bgm->GetCurrent_Time();

#endif

    audioTimer_ = audioSource->GetPlayTimer();

}

//  ハミング窓
//  http://www.densikairo.com/Development/Public/study_dsp/C1EBB4D8BFF4.html
//  https://cognicull.com/ja/qc1y1tr9
std::vector<float> Frequency::HammingWindow(const int& count)
{
    std::vector<float> hm;
    for (int i = 0; i < count; ++i)
    {
        float h;
        //h = 0.54f - (0.46f * cosf((2 * AUDIO_PI * i) / (count - 1)));   //  ハミング窓
		h = 0.42 - 0.5 * cosf(2 * AUDIO_PI * i / (count - 1)) + 0.08 * cosf(4 * AUDIO_PI * i / (count - 1));   //  ブラックマン窓
        hm.emplace_back(h);
    }
    return hm;
}

//  フーリエ変換
void Frequency::FFT(std::vector<Complex>& x)
{
    unsigned int N = x.size(), k = N, n;
    float thetaT = AUDIO_PI_LONG / N;

    //  DFT
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
    //  Decimate
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

//  デバッグ描画
void Frequency::DrawDebug()
{  
    // Plot imageData using ImGui
    ImGui::PlotLines("Amplitude Spectrum", amplitudeSpectrum_.data(), static_cast<int>(amplitudeSpectrum_.size()), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 80));
    ImGui::PlotLines("Old Amplitude Spectrum", oldAmplitudeSpectrum_.data(), static_cast<int>(oldAmplitudeSpectrum_.size()), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 80));
    std::vector<float> squaredValue;
    for (int i = 0; i < amplitudeSpectrum_.size(); ++i)
    {
        float value = amplitudeSpectrum_[i] * amplitudeSpectrum_[i] * 0.000004f;
        squaredValue.emplace_back(value);
    }
    ImGui::PlotLines("Squared Amplitude Spectrum", squaredValue.data(), static_cast<int>(squaredValue.size()), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 80));
    ImGui::PlotLines("Hamming", hamming_.data(), static_cast<int>(hamming_.size()), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 80));
    ImGui::DragFloat("PlayTime", &audioTimer_);
    ImGui::TreePop();
   
}
