#include "BulletHorming.h"

#include "../Nova/Others/MathHelper.h"
#include "../Nova/Core/Framework.h"
#include "Player.h"
#include "../Nova/Collision/Collision.h"

//	�R���X�g���N�^
BulletHorming::BulletHorming(const std::string& filename)
	:Bullet(filename)
{
	//	�G�t�F�N�g�ǂݍ���
	effectResource_[EFFECT::EXPLOSION] = ResourceManager::Instance().LoadEffectResource("./Resources/Effect/Blow11_2.efk");
	effectScale_[EFFECT::EXPLOSION] = 0.3f;

	//	�J�o�[���f��
	DirectX::XMFLOAT4 coverModelColor = { 1.0f,0.0f,0.0f,1.0f };
	coverModel_ = std::make_unique<GltfModelStaticBatching>("./Resources/Model/Cube/source/Cube2.gltf", true, coverModelColor);
	
	//	��������
#if 1
	//coverModel_->GetTransform()->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
#else
	coverModel_->GetTransform()->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
#endif
	//	�ړ����x�ݒ�
	speed_ = 6.0f;

}

//	�f�X�g���N�^
BulletHorming::~BulletHorming()
{

}

//	�X�V����
void BulletHorming::Update(const float& elapsedTime)
{
	//	�ړ�����
	Move(elapsedTime);

	//	�j������
	Destroy(elapsedTime);

	//	�J�o�[���f���X�V����
	CoverModelUpdate(elapsedTime);

}

//	����
void BulletHorming::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	this->direction_ = direction;
	this->GetTransform()->SetPosition(position);

}

//	�j������
void BulletHorming::Destroy(const float& elapsedTime)
{
	//	��������
	lifeTimer_ -= elapsedTime;

	//	���Ɖ~���̓����蔻��
	bool isHitPlayer = false;
	DirectX::XMFLOAT3 bulletPos = this->GetTransform()->GetPosition();
	DirectX::XMFLOAT3 playerPos = Player::Instance().GetTransform()->GetPosition();		//	�v���C���[�̈ʒu
	float playerRadius = Player::Instance().GetRadius();								//	�v���C���[�̔��a
	float playerHeight = Player::Instance().GetHeight();								//	�v���C���[�̍���
	playerPos.y += playerHeight / 2;
	DirectX::XMFLOAT3 outPosition = {};
	isHitPlayer = Collision::IntersectSphereVsCylinder(bulletPos, radius_, playerPos, playerRadius, playerHeight, outPosition);

	//	�������Ԃ��Ȃ��Ȃ邩�A�v���C���[�ɓ����邩�A�v���C���[�ɍU�����ꂽ��
#if 1
	if (lifeTimer_ <= 0.0f || isHitPlayer || damaged_)
#else
	if(damaged_)
#endif
	{
		DirectX::XMFLOAT3 pos = GetTransform()->GetPosition();

		//	�G�t�F�N�g�`��
		effectResource_[EFFECT::EXPLOSION]->Play(pos, effectScale_[EFFECT::EXPLOSION]);
		//effectResource_[EFFECT::EXPLOSION]->PlayAsync(pos, effectScale_[EFFECT::EXPLOSION]);

		//	�������폜
		Bullet::Destroy(elapsedTime);

	}

	if (isHitPlayer)
	{
		Player::Instance().SubtractHp(10);
	}

}

//	�ړ�����
void BulletHorming::Move(const float& elapsedTime)
{
	//	�ړ�
	DirectX::XMFLOAT3 myPos = GetTransform()->GetPosition();
	DirectX::XMFLOAT3 dir;
	target_ = Player::Instance().GetTransform()->GetPosition();
	target_.y += Player::Instance().GetHeight() / 1.5f;			//	�v���C���[�̌��ɓ�����悤�ɂ��邽��
	dir = target_ - myPos;
	dir = Normalize(dir);										//	���K��

	float speed = speed_ * elapsedTime;
	DirectX::XMFLOAT3 position = GetTransform()->GetPosition();

	velocity_ = dir * speed;

	position = position + velocity_;

	GetTransform()->SetPosition(position);

}

//	�`�揈��
void BulletHorming::Render()
{
	//	�e�ۃ��f���`��
	gltfStaticModelResource_->Render();

}

//	�J�o�[���f���`��
void BulletHorming::RnederCoverModel()
{
	float coverScale = BulletManager::Instance().GetCoverScale();
	Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ADD);
	//Graphics::Instance().GetShader()->CreatePsFromCso(Graphics::Instance().GetDevice(), "./Resources/Shader/BulletCoverPS.cso", pixelShader_.ReleaseAndGetAddressOf());
	//coverModel_->SetPixelShader(coverPixelShader_.Get());
	
	//coverModel_->SetPixelShader("./Resources/Shader/GltfModelPS.cso");
	
	coverModel_->SetPixelShaderFromName("./Resources/Shader/BulletCoverPS.cso");
	coverModel_->GetTransform()->SetScaleFactor(coverScale);
	coverModel_->Render();
}

//	�f�o�b�O�`��
void BulletHorming::DrawDebug()
{
	if (ImGui::TreeNode(u8"Bullet �e��"))
	{
		GetTransform()->DrawDebug();
		ImGui::DragFloat("Radius", &radius_, 1.0f, -FLT_MAX, FLT_MAX);				//	���a
		ImGui::DragFloat3("Target", &target_.x, 1.0f, -FLT_MAX, FLT_MAX);			//	�^�[�Q�b�g
		ImGui::DragFloat3("OwnerPos", &ownerPosition_.x, 0.1f, -FLT_MAX, FLT_MAX);	//	���L�҂̈ʒu
		ImGui::DragFloat("Speed", &speed_, 0.5f, -FLT_MAX, FLT_MAX);				//	�e�̑���
		ImGui::DragFloat("LifeTimer", &lifeTimer_, 0.5f, -FLT_MAX, FLT_MAX);		//	��������
		ImGui::TreePop();
	}
}