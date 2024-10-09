#if 0
#include "MetaAI.h"

#include <random>

static Meta* instance = nullptr;
Meta& Meta::Instance()
{
	return *instance;
}

Meta::Meta(Player* player, EnemyManager* eManager) : player_(player), enemyManager_(eManager)
{
	instance = this;
}

void Meta::Update()
{
	//	�����Ŏ����I�Ƀ��^AI���Ď����Ă��鏈��������΋L�ڂ���
	//	���ۑ�ł̓��^AI�̃C�x���g�g���K�[�̓��b�Z�[�W����M�����Ƃ��̂��ߋL�q���鏈���͖���
}

//	���b�Z�[�W��M�����Ƃ��̏���
bool Meta::OnMessage(const Telegram& telegram)
{
	std::vector<Enemy*> enmVec;
	enmVec.clear();
	int enemyCount = enemyManager_->GetEnemyCount();
	switch (telegram.msg)
	{
	case MESSAGE_TYPE::MsgCallHelp:				//	�N�����v���C���[�𔭌������̂ő��̓G���Ă�
		for (int i = 0; i < enemyCount; ++i)	//	�S�Ă̓G�̃��X�g
		{
			int enemyId = enemyManager_->GetEnemy(i)->GetId();
			//	���M�҂������G�l�~�[�Ƀ��b�Z�[�W�𑗂�
			if (telegram.sender != enemyId)
			{
				//	���Ԃ��Ăԃ��b�Z�[�W�̑��M
				//	�N�����v���C���[�𔭌������̂ő��M�҂������G�Ƀ��b�Z�[�W�𑗂�
				//	���^AI����e�G�ɑ��郁�b�Z�[�W�� MsgCallHelp �ł��B

				SendMessaging((int)Meta::Identity::Meta, enemyId, MESSAGE_TYPE::MsgCallHelp);
			}
		}
		return true;
		break;
	case MESSAGE_TYPE::MsgChangeAttackRight:
		//	�G�l�~�[�}�l�[�W������AttackRange�ȓ��̃G�l�~�[�����̔z����쐬��
		for (int i = 0; i < enemyCount; ++i)
		{
			Enemy* enemy = enemyManager_->GetEnemy(i);
			DirectX::XMFLOAT3 position = enemy->GetTransform()->GetPosition();
			DirectX::XMFLOAT3 targetPosition = player_->GetTransform()->GetPosition();
			float vx = targetPosition.x - position.x;
			float vy = targetPosition.y - position.y;
			float vz = targetPosition.z - position.z;
			float dist = sqrtf(vx * vx + vy * vy + vz * vz);

			if (dist < enemy->GetAttackRange())
			{
				enmVec.push_back(enemy);
			}
		}
		if (enmVec.size() > 0)
		{
			//	�U�����̕t�^
			//	enmVec�̃T�C�Y�ŗ������擾���A�擾�����l���C���f�b�N�X�l�Ƃ���
			//	enmVec�ɓo�^���ꂽ�G�̈�l�ɍU������^����
			//	�U������^���郁�b�Z�[�W��MsgGiveAttackRight�ł��B
			//	random �ɂ́A1 �` enmVec.size() �܂ł̗������~����
			int random = 0;												//	�����_���l���i�[
			int randomMax = static_cast<int>(enmVec.size()) - 1;		//	�����_�������͈�(�ő�l)
			int receiver = 0;											//	�󂯎��ID���i�[
			std::random_device rnd;										//	�񌈒�I�ȗ���������𐶐�
			std::mt19937 mt(rnd());										//  �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h�l
			std::uniform_int_distribution<> randEnemy(0, randomMax);	//	[0, randomMax-1] �͈͂̈�l����
			random	= randEnemy(mt) + 1;								//	�����_���l����()�@
			receiver = random * (int)Meta::Identity::Enemy;				//	�󂯎��ID�ݒ�@Enemy��ID��5����A5�������Ă������ߍ��킹��
			SendMessaging((int)Meta::Identity::Meta, receiver, MESSAGE_TYPE::MsgGiveAttackRight);
		}
		return true;
		break;
	case MESSAGE_TYPE::MsgAskAttackRight:	//	�G����A�ŏ��̍U������v�����ꂽ
		int count = 0;						//	�S�Ă̓G�ɂ��āA�U�����������Ă��Ȃ������ׂ�B
		for (int i = 0; i < enemyCount; ++i)
		{
			Enemy* enemy = enemyManager_->GetEnemy(i);
			if (enemy->GetAttackFlg()) count++;
		}
		//	���ɍU���������Ă���G�����Ȃ��Ȃ�A�v�����Ă����G�ɍU������^���郁�b�Z�[�W�𑗂�B
		//	�v�����Ă����G��id��telegram����擾�o���܂��B
		if (count < 1)
		{
			//	�U�����̕t�^
			//	�N���U�����������Ă��Ȃ����MetaAI���瑗�M�҂ɍU�����t�^�̃��b�Z�[�W�𑗂�
			//	���b�Z�[�W��MsgGiveAttackRight
			SendMessaging((int)Meta::Identity::Meta, telegram.sender, MESSAGE_TYPE::MsgGiveAttackRight);
		}
		return true;
		break;
	}
	return false;
}

void Meta::SendMessaging(int sender, int receiver, MESSAGE_TYPE msg)
{
	if (receiver == static_cast<int>(Meta::Identity::Meta))	// MetaAI���̎�
	{
		//	���b�Z�[�W�f�[�^���쐬
		Telegram telegram(sender, receiver, msg);
		//	���b�Z�[�W���M
		OnMessage(telegram);
	}
	else
	{	
		//	�G�l�~�[����M�҂̂Ƃ��A�ǂ̃G�l�~�[�ɑ��M���邩
		//	��M�҂̃|�C���^���擾
		Enemy* receiveEnemy = enemyManager_->GetEnemyFromId(receiver);
		//	���V�[�o�[���Ȃ��Ƃ��֐����I������
		if (receiveEnemy == nullptr) return;
		//	���b�Z�[�W�f�[�^���쐬
		Telegram telegram(sender, receiver, msg);
		//	�f�B���C�������b�Z�[�W�i�����z�����b�Z�[�W�j
		receiveEnemy->OnMessage(telegram);
	}
}
#endif