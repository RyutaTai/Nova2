#include "EnemyManager.h"

#include "../Nova/Collision/Collision.h"
#include "../Nova/Others/MathHelper.h"
#include "Player.h"
#include "Drone.h"

//	�X�V����
void EnemyManager::Update(const float& elapsedTime)
{
	for (Enemy* enemy : enemies_)
	{
		enemy->Update(elapsedTime);
	}

	//	�j������
	for (Enemy* enemy : removes_)
	{
		std::vector<Enemy*>::iterator it =
			std::find(enemies_.begin(), enemies_.end(), enemy);

		if (it != enemies_.end())
		{
			enemies_.erase(it);
		}

		//	�e�ۏ���
		delete enemy;
	}

	//	�j�����X�g���N���A
	removes_.clear();

	//	�G���m�̏Փˏ���
	CollisionEnemyVsEnemies();

}

//	�G�l�~�[���m�̏Փˏ���
//	Unit5 ���Ƌ�

//void EnemyManager::CollisionEnemyVsEnemies()
//{
//	EnemyManager& enemyManager = EnemyManager::Instance();
//
//	//�S�Ă̓G�Ƃ̑�������ŏՓˏ���
//	size_t enemyCount = enemies.size();
//	for (int i = 0; i < enemyCount; i++)
//	{
//		Enemy* enemy = enemies.at(i);
//		for (int n = i+1; n < enemyCount; n++)
//		{
//			Enemy* enemy2 = enemies(n);
//			//�Փˏ���
//			DirectX::XMFLOAT3 outPosition;
//			if (Collision::IntersectSphereVsSphere(
//				enemy->GetPosition(),
//				enemy->GetRadius(),
//				enemy2->GetPosition(),
//				enemy2->GetRadius(),
//				outPosition))
//			{
//				//�����o����̈ʒu�ݒ�
//				enemy2->SetPosition(outPosition);
//			}
//		}
//	}
//}

//	�~���Ɖ~��
void EnemyManager::CollisionEnemyVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();
	int EnemyCount = GetEnemyCount();

	//	�S�Ă̓G�Ƃ̑�������ŏՓˏ���
	for (int i = 0; i < EnemyCount; i++)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);
		for (int n = 0; n < EnemyCount; n++)
		{
			Enemy* enemy2 = enemyManager.GetEnemy(n);
			DirectX::XMFLOAT3 outPosition;
			DirectX::XMFLOAT3 positionOffset = {};
			if (Collision::IntersectCylinderVsCyliner(
				enemy->GetTransform()->GetPosition() + positionOffset,
				enemy->GetRadius(),
				enemy->GetHeight(),
				enemy2->GetTransform()->GetPosition() + positionOffset, 
				enemy2->GetRadius(),
				enemy2->GetHeight(),
				outPosition
			))
			{
				enemy2->GetTransform()->SetPosition(outPosition);
			}
		}
	}
}

//	�G�l�~�[�o�^
void EnemyManager::Register(Enemy* enemy)
{
	enemies_.emplace_back(enemy);
}

//	�G�l�~�[�S�폜
void EnemyManager::Clear()
{
	for (Enemy* enemy : enemies_)
	{
		delete enemy;
	}
	enemies_.clear();
}

//	�G�l�~�[�폜
void EnemyManager::Remove(Enemy* enemy)
{
	//	�j�����X�g�ɒǉ�
	removes_.insert(enemy);
}

//	�h���[������(GameState�ŌĂ�ł���)
void EnemyManager::DroneSpawn(int spawn)
{
	DirectX::XMFLOAT3 playerPos = Player::Instance().GetTransform()->GetPosition();
	DirectX::XMFLOAT3 playerForward = Player::Instance().GetTransform()->CalcForward();
	constexpr float spaceWithP = 2.0f;		//	�v���C���[�Ƃ̊Ԋu
	DirectX::XMFLOAT3 emitter = playerPos + playerForward * spaceWithP;
	DirectX::XMFLOAT3 spawnOffset = { 5.0f,0.0f,5.0f };
	
	for (int spawnCount = 0; spawnCount < spawn; spawnCount++)
	{
		Drone* drone = new Drone();	//	�������ɓo�^�����
		//	�����ʒu�ݒ�
		DirectX::XMFLOAT3 pos =
		{
			emitter.x + spawnOffset.x * spawnCount,
			7.0f,
			emitter.z + spawnOffset.z * spawnCount
		};
		drone->GetTransform()->SetPosition(pos);
		drone->Initialize();	//	�h���[��������
	}
}

//	�`�揈��
void EnemyManager::Render()
{
	for (Enemy* enemy : enemies_)
	{
		enemy->Render();
	}

}

//	�f�o�b�O�v���~�e�B�u�`��
void EnemyManager::DrawDebugPrimitive()
{
	for (Enemy* enemy : enemies_)
	{
		enemy->DrawDebugPrimitive();
	}
}

//	�f�o�b�O�`��
void EnemyManager::DrawDebug()
{
	int enemyCount = static_cast<int>(enemies_.size());

	if (ImGui::TreeNode("EnemyManager"))
	{
		//	EnemyManager�̃f�o�b�O�`��
		ImGui::DragInt("EnemyCount", &enemyCount);	//	�G�l�~�[�̑���

		//	Enemy�̃f�o�b�O�`��
		for (Enemy* enemy : enemies_)
		{
			enemy->DrawDebug();
		}
		ImGui::TreePop();
	}
}