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
	FLOAT32				outerRadius_;	//	�O���a
	FLOAT32				filterParam_;	//	���[�p�X�ɓK������l
};

//	�G�~�b�^�[���
struct SoundEmitter
{
	DirectX::XMFLOAT3	position_;		//	�ʒu
	DirectX::XMFLOAT3	velocity_;		//	�G�~�b�^�[�̑��x
	FLOAT32				minDistance_;	//	�����������n�܂鋗��
	FLOAT32				maxDistance_;	//	������������ő勗��
	float				volume_ = 1.0f;	//	�{�����[��
};

//	
struct SoundDSPSetting
{
	UINT32   srcChannelCount_;				//	�����̃`�����l����
	UINT32   dstChannelCount_;				//	�A�E�g�v�b�g�`�����l����
	FLOAT32  distanceListnerToEmitter_;	//	���X�i�[�ƃG�~�b�^�[�̋���
	FLOAT32  dopplerScale_;					//	�h�b�v���[����
	FLOAT32  radianListenerToEmitter_;	//	���X�i�[����G�~�b�^�[�܂ł̊p�x
	FLOAT32* outputMatrix_;					//	SetOutputMatrix()�ɓn��
	FLOAT32  filterParam_;					//	���[�p�X�ɓK�p����l
};


void DSP(SoundDSPSetting& dspSetting, SoundListener listener, SoundEmitter emitter);