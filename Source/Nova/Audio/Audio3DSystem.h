#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

typedef float FLOAT32;

#define SPEED_OF_SOUND 340.0f

//	リスナー情報
struct SoundListener
{
	DirectX::XMFLOAT3	position_;		//	位置
	DirectX::XMFLOAT3	velocity_;		//	リスナーの速度
	DirectX::XMFLOAT3	frontVec_;		//	前方向
	DirectX::XMFLOAT3	rightVec_;		//	右方向
	FLOAT32				innerRadius_;	//	内半径
	FLOAT32				outer_radius;	//	外半径
	FLOAT32				filter_param;	//	ローパスに適応する値
};

//	エミッター情報
struct SoundEmitter
{
	DirectX::XMFLOAT3	position;		//	位置
	DirectX::XMFLOAT3	velocity;		//	エミッターの速度
	FLOAT32				min_distance;	//	距離減衰が始まる距離
	FLOAT32				max_distance;	//	音が聞こえる最大距離
	float				volume_ = 1.0f;	//	ボリューム
};

//	
struct SoundDSPSetting
{
	UINT32   src_channel_count;				//	音源のチャンネル数
	UINT32   dst_channel_count;				//	アウトプットチャンネル数
	FLOAT32  distance_listner_to_emitter;	//	リスナーとエミッターの距離
	FLOAT32  doppler_scale;					//	ドップラー効果
	FLOAT32  radian_listener_to_emitter;	//	リスナーからエミッターまでの角度
	FLOAT32* output_matrix;					//	SetOutputMatrix()に渡す
	FLOAT32  filter_param;					//	ローパスに適用する値
};


void DSP(SoundDSPSetting& dspSetting, SoundListener listener, SoundEmitter emitter);