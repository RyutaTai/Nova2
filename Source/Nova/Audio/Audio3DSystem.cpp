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
FLOAT32 Angle(DirectX::XMFLOAT3 point_1, DirectX::XMFLOAT3 point_2, DirectX::XMFLOAT3 vector)
{
    DirectX::XMFLOAT3 vector_listner_to_emitter =
    {
        point_1.x - point_2.x,
        point_1.y - point_2.y,
        point_1.z - point_2.z,
    };


    FLOAT32 front_dot = Dot(vector);
    DirectX::XMFLOAT3 front_normalize = { vector.x / front_dot, vector.y / front_dot, vector.z / front_dot };

    FLOAT32 point_dot = Dot(vector_listner_to_emitter);
    DirectX::XMFLOAT3 point_noramlize = { vector_listner_to_emitter.x / point_dot, vector_listner_to_emitter.y / point_dot, vector_listner_to_emitter.z / point_dot };

    return acosf(front_normalize.x * point_noramlize.x + front_normalize.y * point_noramlize.y + front_normalize.z * point_noramlize.z);
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

    // ベクトルの長さを計算
    FLOAT32 length_listener_to_emitter = sqrtf(vector_listner_to_emitter.x * vector_listner_to_emitter.x +
        vector_listner_to_emitter.y * vector_listner_to_emitter.y +
        vector_listner_to_emitter.z * vector_listner_to_emitter.z);

    // 正規化
    DirectX::XMFLOAT3 normalized_listener_to_emitter =
    {
        vector_listner_to_emitter.x / length_listener_to_emitter,
        vector_listner_to_emitter.y / length_listener_to_emitter,
        vector_listner_to_emitter.z / length_listener_to_emitter,
    };

    // ベクトルの長さを計算
    FLOAT32 length_vector = sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);

    // 正規化
    DirectX::XMFLOAT3 normalized_vector = { vector.x / length_vector, vector.y / length_vector, vector.z / length_vector };

    // 内積計算
    FLOAT32 dot_product = normalized_listener_to_emitter.x * normalized_vector.x +
        normalized_listener_to_emitter.y * normalized_vector.y +
        normalized_listener_to_emitter.z * normalized_vector.z;

    // acosf関数の範囲チェック
    dot_product = max(-1.0f, min(1.0f, dot_product));

    // 角度計算
    return acosf(dot_product);
}
#endif

void DSP(SoundDSPSetting& dspSetting, SoundListener listener, SoundEmitter emitter)
{
    //  距離
    dspSetting.distance_listner_to_emitter = VECTOR3Length(emitter.position, listener.position_);

    // ドップラー効果
    dspSetting.doppler_scale = (SPEED_OF_SOUND - (listener.velocity_.x + listener.velocity_.y + listener.velocity_.z)) /
                                    (SPEED_OF_SOUND - (emitter.velocity.x + emitter.velocity.y + emitter.velocity.z));

    //  角度
    dspSetting.radian_listener_to_emitter = (Angle(emitter.position, listener.position_,  listener.rightVec_) < M_PI * 0.5f) ?
                                                Angle(emitter.position, listener.position_, listener.frontVec_) : -Angle(emitter.position, listener.position_, listener.frontVec_);

    // 音の減衰率
    FLOAT32 scaler = max(0.0f, min(1.0f, 1.0f - dspSetting.distance_listner_to_emitter / emitter.max_distance));

    switch (dspSetting.src_channel_count * dspSetting.dst_channel_count)
    {
    case 1:
        dspSetting.output_matrix[0] = scaler;
        break;

    case 4:
        FLOAT32 angle = (Angle(emitter.position, listener.position_, listener.rightVec_) < M_PI * 0.5f) ?
            dspSetting.radian_listener_to_emitter : -Angle(emitter.position, listener.position_, listener.frontVec_);
        //angle = (dsp_setting.radian_listener_to_emitter + 90) * 0.5f;
        angle = (dspSetting.radian_listener_to_emitter + M_PI_2) * 0.5f;


        FLOAT32 L = cosf(angle);
        FLOAT32 R = sinf(angle);
        if (dspSetting.distance_listner_to_emitter > emitter.min_distance)
        {
            L *= scaler;
            R *= scaler;
        }

        //dsp_setting.output_matrix[0] = dsp_setting.output_matrix[1] = L;    //  元のコード
        //dsp_setting.output_matrix[2] = dsp_setting.output_matrix[3] = R;

        //  変更したら直った。
        dspSetting.output_matrix[0] = dspSetting.output_matrix[2] = L;    //  左を0、2に変更
        dspSetting.output_matrix[1] = dspSetting.output_matrix[3] = R;    //  右を1、3に変更

#if 0   //  dsp_setting.output_matrixの値が生きているか確認
        float pan = -90.0f;	//	真左
        float rad = ((-90.0f + 90.0f) / 2.0f) * (M_PI / 180.0f);	//  ラジアンに変換
        dspSetting.output_matrix[0] = cosf(rad);				    //  左ボリューム
        dspSetting.output_matrix[2] = cosf(rad);				    //  左ボリューム
        dspSetting.output_matrix[1] = sinf(rad);				    //  右ボリューム
        dspSetting.output_matrix[3] = sinf(rad);				    //  右ボリューム

#endif


        break;
    }


    // リスナーと音源の角度からローパスに適用する値を計算
    dspSetting.filter_param = (std::abs(dspSetting.radian_listener_to_emitter) > listener.innerRadius_) ?
        listener.filter_param * min(1.0f, (std::abs(dspSetting.radian_listener_to_emitter) - listener.innerRadius_) / (listener.outer_radius - listener.innerRadius_)) :
        dspSetting.filter_param = 0.0f;
}
