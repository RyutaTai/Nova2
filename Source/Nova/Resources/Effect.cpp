#include "Effect.h"

#include "EffectManager.h"
#include "../Graphics/Graphics.h"

//	�R���X�g���N�^
Effect::Effect(const char* fileName)
{
	//	�G�t�F�N�g��ǂݍ��݂���O�Ƀ��b�N����
	//	���}���`�X���b�h��Effect���쐬�����DeviceContext�𓯎��A�N�Z�X����
	//	�t���[�Y����\��������̂Ŕr�����䂷��
	std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());

	//	Effekseer�̃��\�[�X��ǂݍ���
	//	Effekseer��UTF-16�̃t�@�C���p�X�ȊO�͑Ή����Ă��Ȃ����ߕ����R�[�h�ϊ����K�v
	char16_t utf16FileName[256];
	Effekseer::ConvertUtf8ToUtf16(utf16FileName, 256, fileName);

	//	Effekseer::Manager���擾
	Effekseer::ManagerRef effekseerManager_ = EffectManager::Instance().GetEffecseerManager();

	//	Effekseer�G�t�F�N�g��ǂݍ���
	effekseerEffect_ = Effekseer::Effect::Create(effekseerManager_, (EFK_CHAR*)utf16FileName);

	_ASSERT_EXPR(effekseerEffect_.Get(), L"Failed to load effect.");

}

//	��~
void Effect::Stop(Effekseer::Handle handle)
{
	Effekseer::ManagerRef effekseerManager_ = EffectManager::Instance().GetEffecseerManager();

	effekseerManager_->StopEffect(handle);
}

//	���W�ݒ�
void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
	Effekseer::ManagerRef effekseerManager_ = EffectManager::Instance().GetEffecseerManager();

	effekseerManager_->SetLocation(handle, position.x, position.y, position.z);
}

//	�X�P�[���ݒ�
void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
	Effekseer::ManagerRef effekseerManager_ = EffectManager::Instance().GetEffecseerManager();

	effekseerManager_->SetScale(handle, scale.x, scale.y, scale.z);
}

//	�Đ�
Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& position, float scale)
{
	_ASSERT_EXPR(effekseerEffect_.Get(), L"Failed to load effect.");

	Effekseer::ManagerRef effekseerManager_ = EffectManager::Instance().GetEffecseerManager();

	Effekseer::Handle handle = effekseerManager_->Play(effekseerEffect_, position.x, position.y, position.z);
	effekseerManager_->SetScale(handle, scale, scale, scale);
	return handle;
}

//	�v���C���X���b�h����������
void Effect::PlayAsync(const DirectX::XMFLOAT3& position, float scale)
{
	_ASSERT_EXPR(effekseerEffect_.Get(), L"Failed to load effect.");

	//	���b�N���ăv���C�����s
	std::lock_guard<std::mutex> lock(playMutex_);

	Effekseer::ManagerRef effekseerManager_ = EffectManager::Instance().GetEffecseerManager();
	Effekseer::Handle handle = effekseerManager_->Play(effekseerEffect_, position.x, position.y, position.z);
	effekseerManager_->SetScale(handle, scale, scale, scale);
}


