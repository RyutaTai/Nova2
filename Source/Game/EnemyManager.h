#pragma once

#include <vector>
#include <set>

#include "Enemy.h"

class EnemyManager
{
private:
	EnemyManager() {}
	~EnemyManager() {}

public:
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}

	void Update(const float& elapsedTime);
	void Render();

	void Register(Enemy* enemy);		//	�G�l�~�[�o�^
	void Clear();						//	�G�l�~�[�S�폜
	void Remove(Enemy* enemy);			//	�G�l�~�[�폜
	void DroneSpawn(int spawn);			//	�h���[������
	
	void DrawDebug();
	void DrawDebugPrimitive();

	Enemy*				GetEnemy(int index)		{ return enemies_.at(index); }					//	�G�l�~�[�擾
	std::vector<Enemy*> GetEnemies()			{ return enemies_; }							//	�G�l�~�[�擾
	int					GetEnemyCount() const	{ return static_cast<int>(enemies_.size()); }	//	�G�l�~�[���擾

private:
	void CollisionEnemyVsEnemies();		//	�G�l�~�[���m�̏Փ˔���

private:
	std::vector	<Enemy*> enemies_ = {};		//	�G�l�~�[�̔z��
	std::set	<Enemy*> removes_ = {};		//	�j������G�l�~�[�̔z�� ;

};
