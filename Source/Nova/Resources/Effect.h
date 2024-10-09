#pragma once

#include <DirectXMath.h>
#include <Effekseer/Effekseer.h>
#include <thread>
#include <mutex>

//	�G�t�F�N�g
class Effect
{
public:
	Effect(const char* filename);
	~Effect() {};

	//	�Đ�
	Effekseer::Handle Play(const DirectX::XMFLOAT3& position, float scale = 1.0f);

	//	��~
	void Stop(Effekseer::Handle handle);

	//	���W�ݒ�
	void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);

	//	�X�P�[���ݒ�
	void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

	//	�Đ��̃X���b�h��
	void PlayAsync(const DirectX::XMFLOAT3& position, float scale = 1.0f);

private:
	Effekseer::EffectRef	effekseerEffect_;
	std::mutex				playMutex_;			// �X���b�h�ōĐ����s�����߂̃��b�N

};
