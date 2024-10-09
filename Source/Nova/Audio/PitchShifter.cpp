#include "PitchShifter.h"

PitchShifter::PitchShifter()
{

}

//	�s�b�`�V�t�g�K��
void PitchShifter::ApplyPitchShift(float pitchShift, const std::vector<float>& input, std::vector<float>& output)
{
    std::vector<float> window = frequency_->HammingWindow(FRAME_SIZE);  //  �n�~���O������

    int numFrames = (input.size() - FRAME_SIZE) / OVERLAP + 1;
    output.resize(input.size() + (numFrames - 1) * OVERLAP);

    for (int frame = 0; frame < numFrames; ++frame)
    {
        int offset = frame * OVERLAP;
        std::vector<Complex> timeDomain(FRAME_SIZE);
        std::vector<Complex> freqDomain(FRAME_SIZE);

        // �t���[�������o���ăE�B���h�E��K�p
        for (int i = 0; i < FRAME_SIZE; ++i) 
        {
            timeDomain[i] = Complex(input[offset + i] * window[i], 0);
        }

        // �t���[����FFT�ɂ����g���̈�֕ϊ�
        frequency_->FFT(timeDomain);

        // �s�b�`�V�t�g�̓K�p
        int newSize = static_cast<int>(FRAME_SIZE / pitchShift);
        std::vector<Complex> shiftedFreqDomain(newSize);

        for (int i = 0; i < FRAME_SIZE / 2 + 1; ++i)
        {
            int shiftedIndex = static_cast<int>(i * pitchShift);
            if (shiftedIndex < newSize) 
            {
                shiftedFreqDomain[shiftedIndex] = timeDomain[i];
            }
        }

        // �[���p�f�B���O
        for (int i = newSize; i < FRAME_SIZE; ++i) 
        {
            shiftedFreqDomain.push_back(Complex(0, 0));
        }

        // �tFFT�ɂ�莞�ԗ̈�֖߂�
        frequency_->FFT(shiftedFreqDomain); // Inverse FFT (����F�����֐����t�����ɂ��g����)

        // �E�B���h�E���ēK�p���ăI�[�o�[���b�v���Z
        for (int i = 0; i < FRAME_SIZE; ++i) 
        {
            output[offset + i] += shiftedFreqDomain[i].real() * window[i];
        }
    }
}
