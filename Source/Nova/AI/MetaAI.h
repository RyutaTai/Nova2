#pragma once
#if 0
#include <set>

#include "Telegram.h"
#include "../../Game/Player.h"
#include "../../Game/EnemyManager.h"

class Meta
{
private:
	Player*			player_			= nullptr;
	EnemyManager*	enemyManager_	= nullptr;

public:
	enum class Identity : int
	{
		Meta	= 0,	//	MetaAI
		Player	= 1,	//	�v���C���[ID��1�`4
		Enemy	= 5		//	�G�l�~�[ID��5�ȍ~��t�^
	};
	//	�R���X�g���N�^
	Meta(Player* player, EnemyManager* eManager);
	~Meta() {}
	//	�C���X�^���X�擾
	static Meta& Instance();
	//	MetaAI�̋@�\��ǉ����Ă���
	//	�X�V����
	void Update();
	//	���b�Z�[�W��M�����Ƃ��̏���
	bool OnMessage(const Telegram& msg);
	//	���b�Z�[�W���M�֐�
	void SendMessaging(int sender, int receiver, MESSAGE_TYPE msg);

};
#endif