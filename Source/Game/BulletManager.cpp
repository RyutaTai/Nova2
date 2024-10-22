#include "BulletManager.h"

#include "../../External/imgui/imgui.h"

//	�R���X�g���N�^
BulletManager::BulletManager()
{
	
}

//	�f�X�g���N�^
BulletManager::~BulletManager()
{
	Clear();
}

//	����������
void BulletManager::Initialize()
{
	for (Bullet* bullet : bullets_)
	{
		bullet->Initialize();
	}
}

//	�X�V����
void BulletManager::Update(const float& elapsedTime)
{
	//	�X�V����
	for (Bullet* bullet : bullets_)
	{
		bullet->Update(elapsedTime);
	}

	//	�j������
	//	��bullets�͈̔�for������erase()����ƕs����������邽��,
	//	�X�V�������I�������ɁA�j�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����
	for (Bullet* remove : removes_)
	{
		//	std::vector����v�f��j������Ƃ��̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
		//	std::vector�ŊǗ�����Ă���v�f���폜����ɂ�erase()�֐����g�p����
		//	�j�����X�g�̃|�C���^����C�e���[�^�[���������Aerase()�ɓn��
		std::vector	<Bullet*>::iterator it =
			std::find(bullets_.begin(), bullets_.end(), remove);

		if (it != bullets_.end())
		{
			bullets_.erase(it);
		}

		//	�e�ۏ���
		delete remove;
	}
	//	�j�����X�g���N���A
	removes_.clear();
}

//	�J�o�[���f���X�V����
void BulletManager::CoverModelUpdate(const float& elapsedTime)
{
	//	�X�P�[���ݒ�(BGM�ɍ����悤�ɂ�����)���͉��ň������ŕω�������
	if (isCoverModelUpdate_)
	{
		//	�X�P�[�����Z
#if 0
		coverScale_ += scaleAdd_ * elapsedTime;
		if (coverScale_ > COVER_SCALE_MAX)
		{
			coverScale_ = COVER_SCALE_MIN;
			scaleAdd_ = -scaleAdd_;
		}
		if (coverScale_ < COVER_SCALE_MIN)
		{
			coverScale_ = COVER_SCALE_MIN;
			scaleAdd_ = -scaleAdd_;
		}
#endif

		//	�X�P�[�����Z
#if 1
		coverScale_ -= scaleAdd_ * elapsedTime;
		if (coverScale_ > COVER_SCALE_MAX)
		{
			coverScale_ = COVER_SCALE_MAX;
			scaleAdd_ = -scaleAdd_;
		}
		if (coverScale_ < COVER_SCALE_MIN)
		{
			coverScale_ = COVER_SCALE_MAX;
			scaleAdd_ = -scaleAdd_;
		}
#endif

	}

	for (Bullet* bullet : bullets_)
	{
		bullet->CoverModelUpdate(elapsedTime);
	}

}

//	�`�揈��
void BulletManager::Render()
{
	//	�e�ە`��
	for (Bullet* bullet : bullets_)
	{
		bullet->Render();
	}
	//	�e�ۃJ�o�[���f���`��
	if (coverModelDraw_)
	{
		for (Bullet* bullet : bullets_)	//	���������͍Ō�ɌĂ�(���̌�Ƀ��f����`�悵�Ă��A�������D�悳��Ă��܂�)
		{
			bullet->RnederCoverModel();
		}
	}
}

//	�e�ۓo�^
void BulletManager::Register(Bullet* bullet)
{
	bullets_.emplace_back(bullet);
}

//	�e�ۑS�폜
void BulletManager::Clear()
{
	for (Bullet* bullet : bullets_)
	{
		delete bullet;
	}
	bullets_.clear();
}

//	�e�ۍ폜
void BulletManager::Remove(Bullet* bullet)
{
	//	�j�����X�g�ɒǉ�
	//	����projectiles�̗v�f���폜����ƁA�͈�for�����ŕs����N�������ߔj�����X�g�ɒǉ�����
	removes_.insert(bullet);
}

//	�f�o�b�O�v���~�e�B�u�`��
void BulletManager::DrawDebugPrimitive()
{
	for (Bullet* bullet : bullets_)
	{
		bullet->DrawDebugPrimitive();
	}
}

//	�f�o�b�O�`��
void BulletManager::DrawDebug()
{
	if (ImGui::TreeNode(u8"BulletManager"))
	{
		ImGui::Checkbox("Bullet Destroy ", &isBulletDestroy_);						//	�e�۔j��
		ImGui::Checkbox("CoverDraw ", &coverModelDraw_);							//	�J�o�[���f���`��
		ImGui::Checkbox("IsCoverModelUpdate", &isCoverModelUpdate_);				//	�J�o�[���f���X�V����(�f�t�H���g��true)
		ImGui::DragFloat("ScaleAdd", &scaleAdd_, 1.0f, -FLT_MAX, FLT_MAX);			//	�J�o�[�X�P�[���̑�����
		ImGui::DragFloat("CoverScale", &coverScale_, 1.0f, -FLT_MAX, FLT_MAX);		//	���a
		ImGui::DragFloat("CoverScaleTimer", &coverScaleTimer_, 1.0f, -FLT_MAX, FLT_MAX);
		ImGui::TreePop();
	}

	if (isBulletDestroy_)	//	�e�۔j��
	{
		Clear();
	}

	for (Bullet* bullet : bullets_)
	{
		bullet->DrawDebug();
	}
}