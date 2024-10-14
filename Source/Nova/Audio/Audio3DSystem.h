#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

typedef float FLOAT32;

#define SPEED_OF_SOUND 340.0f

//	���X�i�[���
struct SoundListener
{
	DirectX::XMFLOAT3	position_;		//	�ʒu
	DirectX::XMFLOAT3	velocity_;		//	���X�i�[�̑��x
	DirectX::XMFLOAT3	frontVec_;		//	�O����
	DirectX::XMFLOAT3	rightVec_;		//	�E����
	FLOAT32				innerRadius_;	//	�����a
	FLOAT32				outer_radius;	//	�O���a
	FLOAT32				filter_param;	//	���[�p�X�ɓK������l
};

//	�G�~�b�^�[���
struct SoundEmitter
{
	DirectX::XMFLOAT3	position;		//	�ʒu
	DirectX::XMFLOAT3	velocity;		//	�G�~�b�^�[�̑��x
	FLOAT32				min_distance;	//	�����������n�܂鋗��
	FLOAT32				max_distance;	//	������������ő勗��
	float				volume_ = 1.0f;	//	�{�����[��
};

//	
struct SoundDSPSetting
{
	UINT32   src_channel_count;				//	�����̃`�����l����
	UINT32   dst_channel_count;				//	�A�E�g�v�b�g�`�����l����
	FLOAT32  distance_listner_to_emitter;	//	���X�i�[�ƃG�~�b�^�[�̋���
	FLOAT32  doppler_scale;					//	�h�b�v���[����
	FLOAT32  radian_listener_to_emitter;	//	���X�i�[����G�~�b�^�[�܂ł̊p�x
	FLOAT32* output_matrix;					//	SetOutputMatrix()�ɓn��
	FLOAT32  filter_param;					//	���[�p�X�ɓK�p����l
};


void DSP(SoundDSPSetting& dspSetting, SoundListener listener, SoundEmitter emitter);