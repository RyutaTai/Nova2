#include "Audio3DSystem.h"

#include <corecrt_math_defines.h>

inline FLOAT32 VECTOR3Length(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
{
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

inline FLOAT32 Dot(DirectX::XMFLOAT3 a)
{
    return sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}

#if 1
FLOAT32 Angle(DirectX::XMFLOAT3 point1, DirectX::XMFLOAT3 point2, DirectX::XMFLOAT3 vector)
{
    DirectX::XMFLOAT3 vectorListnerToEmitter =
    {
        point1.x - point2.x,
        point1.y - point2.y,
        point1.z - point2.z,
    };


    FLOAT32 frontDot = Dot(vector);
    DirectX::XMFLOAT3 frontNormalize = { vector.x / frontDot, vector.y / frontDot, vector.z / frontDot };

    FLOAT32 pointDot = Dot(vectorListnerToEmitter);
    DirectX::XMFLOAT3 pointNoramlize = { vectorListnerToEmitter.x / pointDot, vectorListnerToEmitter.y / pointDot, vectorListnerToEmitter.z / pointDot };

    return acosf(frontNormalize.x * pointNoramlize.x + frontNormalize.y * pointNoramlize.y + frontNormalize.z * pointNoramlize.z);
}
#else
FLOAT32 Angle(DirectX::XMFLOAT3 point_1, DirectX::XMFLOAT3 point_2, DirectX::XMFLOAT3 vector)
{
    DirectX::XMFLOAT3 vector_listner_to_emitter =
    {
        point_1.x - point_2.x,
        point_1.y - point_2.y,
        point_1.z - point_2.z,
    };

    // �x�N�g���̒������v�Z
    FLOAT32 length_listener_to_emitter = sqrtf(vector_listner_to_emitter.x * vector_listner_to_emitter.x +
        vector_listner_to_emitter.y * vector_listner_to_emitter.y +
        vector_listner_to_emitter.z * vector_listner_to_emitter.z);

    // ���K��
    DirectX::XMFLOAT3 normalized_listener_to_emitter =
    {
        vector_listner_to_emitter.x / length_listener_to_emitter,
        vector_listner_to_emitter.y / length_listener_to_emitter,
        vector_listner_to_emitter.z / length_listener_to_emitter,
    };

    // �x�N�g���̒������v�Z
    FLOAT32 length_vector = sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);

    // ���K��
    DirectX::XMFLOAT3 normalized_vector = { vector.x / length_vector, vector.y / length_vector, vector.z / length_vector };

    // ���όv�Z
    FLOAT32 dot_product = normalized_listener_to_emitter.x * normalized_vector.x +
        normalized_listener_to_emitter.y * normalized_vector.y +
        normalized_listener_to_emitter.z * normalized_vector.z;

    // acosf�֐��͈̔̓`�F�b�N
    dot_product = max(-1.0f, min(1.0f, dot_product));

    // �p�x�v�Z
    return acosf(dot_product);
}
#endif

void DSP(SoundDSPSetting& dspSetting, SoundListener listener, SoundEmitter emitter)
{
    //  ����
    dspSetting.distanceListnerToEmitter_ = VECTOR3Length(emitter.position_, listener.position_);

    // �h�b�v���[����
    dspSetting.dopplerScale_ = (SPEED_OF_SOUND - (listener.velocity_.x + listener.velocity_.y + listener.velocity_.z)) /
                                    (SPEED_OF_SOUND - (emitter.velocity_.x + emitter.velocity_.y + emitter.velocity_.z));

    //  �p�x
    dspSetting.radianListenerToEmitter_ = (Angle(emitter.position_, listener.position_,  listener.rightVec_) < M_PI * 0.5f) ?
                                                Angle(emitter.position_, listener.position_, listener.frontVec_) : -Angle(emitter.position_, listener.position_, listener.frontVec_);

    // ���̌�����
    FLOAT32 scaler = max(0.0f, min(1.0f, 1.0f - dspSetting.distanceListnerToEmitter_ / emitter.maxDistance_));

    switch (dspSetting.srcChannelCount_ * dspSetting.dstChannelCount_)
    {
    case 1:
        dspSetting.outputMatrix_[0] = scaler;
        break;

    case 4:
        FLOAT32 angle = (Angle(emitter.position_, listener.position_, listener.rightVec_) < M_PI * 0.5f) ?
            dspSetting.radianListenerToEmitter_ : -Angle(emitter.position_, listener.position_, listener.frontVec_);
        //angle = (dsp_setting.radian_listener_to_emitter + 90) * 0.5f;
        angle = (dspSetting.radianListenerToEmitter_ + M_PI_2) * 0.5f;

        FLOAT32 L = cosf(angle);
        FLOAT32 R = sinf(angle);
        if (dspSetting.distanceListnerToEmitter_ > emitter.minDistance_)
        {
            L *= scaler;
            R *= scaler;
        }

        //dsp_setting.output_matrix[0] = dsp_setting.output_matrix[1] = L;    //  ���̃R�[�h
        //dsp_setting.output_matrix[2] = dsp_setting.output_matrix[3] = R;

        //  �ύX�����璼�����B
        dspSetting.outputMatrix_[0] = dspSetting.outputMatrix_[2] = L;    //  ����0�A2�ɕύX
        dspSetting.outputMatrix_[1] = dspSetting.outputMatrix_[3] = R;    //  �E��1�A3�ɕύX

#if 0   //  dsp_setting.output_matrix�̒l�������Ă��邩�m�F
        float pan = -90.0f;	//	�^��
        float rad = ((-90.0f + 90.0f) / 2.0f) * (M_PI / 180.0f);	//  ���W�A���ɕϊ�
        dspSetting.output_matrix[0] = cosf(rad);				    //  ���{�����[��
        dspSetting.output_matrix[2] = cosf(rad);				    //  ���{�����[��
        dspSetting.output_matrix[1] = sinf(rad);				    //  �E�{�����[��
        dspSetting.output_matrix[3] = sinf(rad);				    //  �E�{�����[��

#endif


        break;
    }


    // ���X�i�[�Ɖ����̊p�x���烍�[�p�X�ɓK�p����l���v�Z
    dspSetting.filterParam_ = (std::abs(dspSetting.radianListenerToEmitter_) > listener.innerRadius_) ?
        listener.filterParam_ * min(1.0f, (std::abs(dspSetting.radianListenerToEmitter_) - listener.innerRadius_) / (listener.outerRadius_ - listener.innerRadius_)) :
        dspSetting.filterParam_ = 0.0f;
}
