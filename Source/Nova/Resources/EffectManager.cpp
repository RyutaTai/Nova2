#include "EffectManager.h"

#include "../Graphics/Graphics.h"

//	������
void EffectManager::Initialize()
{
	Graphics& graphics = Graphics::Instance();

	//	Effekseer�����_������
	effekseerRenderer_ = EffekseerRendererDX11::Renderer::Create(graphics.GetDevice(), graphics.GetDeviceContext(), 2048);

	//	Effekseer�}�l�[�W���[����
	effekseerManager_ = Effekseer::Manager::Create(2048);

	//	Effekseer�����_���̊e�퐶��(���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͒�^�I�Ɉȉ��̐ݒ��OK)
	effekseerManager_->SetSpriteRenderer(effekseerRenderer_->CreateSpriteRenderer());
	effekseerManager_->SetRibbonRenderer(effekseerRenderer_->CreateRibbonRenderer());
	effekseerManager_->SetRingRenderer(effekseerRenderer_->CreateRingRenderer());
	effekseerManager_->SetTrackRenderer(effekseerRenderer_->CreateTrackRenderer());
	effekseerManager_->SetModelRenderer(effekseerRenderer_->CreateModelRenderer());
	//	Effekseer���ł̃��[�_�[�̐ݒ�(���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͒�^�I�Ɉȉ��̐ݒ��OK)
	effekseerManager_->SetTextureLoader(effekseerRenderer_->CreateTextureLoader());
	effekseerManager_->SetModelLoader(effekseerRenderer_->CreateModelLoader());
	effekseerManager_->SetMaterialLoader(effekseerRenderer_->CreateMaterialLoader());

	//	Effekseer��������W�n�Ōv�Z����
	effekseerManager_->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

}

//	�I����
void EffectManager::Finalize()
{
	//	EffektManager�Ȃǂ̓X�}�[�g�|�C���^�ɂ���Ĕj�������̂ŉ������Ȃ�
}

//	�X�V����
void EffectManager::Update(const float& elapsedTime)
{
	//	�G�t�F�N�g�X�V����(�����ɂ̓t���[���̌o�ߎ��Ԃ�n��)
	effekseerManager_->Update(elapsedTime * 60.0f);
}

//	�`�揈��
void EffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	//	�r���[&�v���W�F�N�V�����s���Effekseer�����_���ɐݒ�
	effekseerRenderer_->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
	effekseerRenderer_->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

	//	Effekseer�`��J�n
	effekseerRenderer_->BeginRendering();

	//	Effekseer�`����s
	//	�}�l�[�W���[�P�ʂŕ`�悷��̂ŕ`�揇�𐧌䂷��ꍇ�̓}�l�[�W���[�𕡐��쐬���A
	//	Draw()�֐������s���鏇���Ő���ł�����
	effekseerManager_->Draw();

	//	Effekseer�`��I��
	effekseerRenderer_->EndRendering();
}