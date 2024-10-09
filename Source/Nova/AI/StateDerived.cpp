#if 0
#include "StateDerived.h"

#include "MetaAI.h"
#include "../Others/MathHelper.h"
#include "../../Game/Player.h"

//	サーチステートデストラクタ
SearchState<class T>::~SearchState()
{
	for (State<T>* state : subStatePool)
	{
		delete state;
	}
	subStatePool.clear();
}

//	サーチステートに入った時のメソッド
void SearchState<class T>::Initialize()
{
	SetSubState(static_cast<int>(EnemySlime::Search::Idle));
}

//	サーチステートで実行するメソッド
void SearchState<class T>::Update(float elapsedTime)
{
	subState->Update(elapsedTime);
}

//	サーチステートから出ていくときのメソッド
void SearchState<class T>::Finalize()
{

}

//	バトルステートデストラクタ
BattleState<class T>::~BattleState()
{
	for (State* state : subStatePool)
	{
		delete state;
	}
	subStatePool.clear();
}

//	バトルステートに入った時のメソッド
void BattleState<class T>::Initialize()
{
	SetSubState(static_cast<int>(EnemySlime::Battle::Attack));
}

//	バトルステートで実行するメソッド
void BattleState<class T>::Update(float elapsedTime)
{
	subState->Update(elapsedTime);
}

//	バトルステートから出ていくときのメソッド
void BattleState<class T>::Finalize()
{

}

//	レシーブステートのデストラクタ
RecieveState<class T>::~RecieveState()
{
	for (State* state : subStatePool)
	{
		delete state;
	}
	subStatePool.clear();
}

//	レシーブステートに入った時のメソッド
//void RecieveState<class T>::Initialize()
//{
//	//	初期ステートを設定
//	SetSubState(static_cast<int>(EnemySlime::Recieve::Called));
//}

template<class T>
void RecieveState<T>::Initialize()
{
	owner_->SearchPlayer();
}

//	レシーブステートで実行するメソッド
void RecieveState<class T>::Update(float elapsedTime)
{
	//	子ステート実行
	subState->Update(elapsedTime);
	//	プレイヤー索敵
	if (owner_->SearchPlayer())
	{
		//	Battleステートへ遷移
		owner_->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Battle));
	}
}

//	レシーブステートから出ていくときのメソッド
void RecieveState<class T>::Finalize()
{

}

//	徘徊ステートに入った時のメソッド
void WanderState<class Enemy>::Initizalize()
{
	owner_->SetRandomTargetPosition();
	owner_->SetAnimation(static_cast<int>(EnemyAnimation::RunFWD), true);
}

//	徘徊ステートで実行するメソッド
void WanderState<class Enemy>::Update(float elapsedTime)
{
	//	目的地点までのXZ平面での距離判定
	DirectX::XMFLOAT3 position = owner_->GetTransform()->GetPosition();
	DirectX::XMFLOAT3 targetPosition = owner_->GetTargetPosition();
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;

	//	目的地へ着いた
	float radius = owner_->GetRadius();
	if (distSq < radius * radius)
	{
		//	待機ステートへ遷移
		//	ChangeSubStateクラスで2層目のStateを切り替える
		owner_->GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Search::Idle));
	}
	//	目的地点へ移動
	owner_->MoveToTarget(elapsedTime, 0.5f);
	//	プレイヤー索敵
	if (owner_->SearchPlayer())
	{
		//	見つかったら追跡ステートへ遷移
		//	ChangeStateクラスで1層の目Stateを切り替える
		owner_->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Battle));
		owner_->GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Battle::Pursuit));
	}

}

//	徘徊ステートから出ていくときのメソッド
void WanderState<class T>::Finalize()
{

}

//	待機ステートに入った時のメソッド
void IdleState<class Enemy>::Initizalize()
{
	owner_->SetAnimation(static_cast<int>(EnemyAnimation::IdleNormal), true);
	//	タイマーをランダム設定
	owner_->SetStateTimer(Mathf::RandomRange(3.0f, 5.0f));
}

//	待機ステートで実行するメソッド
void IdleState<class T>::Update(float elapsedTime)
{
	//	タイマー処理
	owner_->SetStateTimer(owner_->GetStateTimer() - elapsedTime);

	//	待機時間が経過したとき徘徊ステートへ遷移
	if (owner_->GetStateTimer() < 0.0f)
	{
		owner_->GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Search::Wander));
	}

	//	プレイヤーが見つかったとき追跡ステートへ遷移
	if (owner_->SearchPlayer())
	{
		owner_->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Battle));
		owner_->GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Battle::Pursuit));
	}

}

//	待機ステートから出ていくときのメソッド
void IdleState<class T>::Finalize()
{

}

//	追跡ステートに入った時のメソッド
void PursuitState<class T>::Initialize()
{
	owner_->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::RunFWD), true);
	//	数秒間追跡するタイマーをランダム設定
	owner_->SetStateTimer(Mathf::RandomRange(3.0f, 5.0f));
	//	敵を発見したため仲間を呼ぶ
	//	エネミーからメタAIへMsgCallHelpを送信する。
	Meta::Instance().SendMessaging(owner_->GetId(), (int)Meta::Identity::Meta, MESSAGE_TYPE::MsgCallHelp);
}

//	追跡ステートで実行するメソッド
void PursuitState<class T>::Update(float elapsedTime)
{
	//	目標地点をプレイヤー位置に設定
	owner_->SetTargetPosition(Player::Instance().GetTransform()->GetPosition());

	//	目的地点へ移動
	owner_->MoveToTarget(elapsedTime, 1.0);

	//	タイマー処理
	owner_->SetStateTimer(owner_->GetStateTimer() - elapsedTime);

	//	追跡時間が経過したとき待機ステートへ遷移
	if (owner_->GetStateTimer() < 0.0f)
	{
		owner_->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Search));
	}

	float vx = owner_->GetTargetPosition().x - owner_->GetTransform()->GetPosition().x;
	float vy = owner_->GetTargetPosition().y - owner_->GetTransform()->GetPosition().y;
	float vz = owner_->GetTargetPosition().z - owner_->GetTransform()->GetPosition().z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	//	攻撃範囲に入ったとき攻撃ステートへ遷移
	if (dist < owner_->GetAttackRange())
	{
		owner_->GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Battle::Attack));
	}
}

//	追跡ステートから出ていくときのメソッド
void PursuitState<class T>::Finalize()
{

}

//	攻撃ステートに入った時のメソッド
void AttackState<class T>::Initialize()
{
	//	攻撃権がなければ
	if (!owner_->GetAttackFlg())
	{
		//	攻撃権をメタAIから求める
		//	エネミーからメタAIへMsgAskAttackRightを送信する
		Meta::Instance().SendMessaging(owner_->GetId(), (int)Meta::Identity::Meta, MESSAGE_TYPE::MsgAskAttackRight);
	}
	//	攻撃権があればモーション再生開始
	if (owner_->GetAttackFlg())
	{
		owner_->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::Attack01), false);
	}
}

//	攻撃ステートで実行するメソッド
void AttackState<class T>::Update(float elapsedTime)
{
	//	攻撃権があるとき 
	if (owner_->GetAttackFlg())
	{
		//	攻撃モーションが終わっていれば戦闘中待機へ遷移 
		if (!owner_->GetModel()->IsPlayAnimation())
		{
			owner_->GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Battle::Standby));
		}
	}
	else
	{
		//	攻撃権がないときステート変更 
		owner_->GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Battle::Standby));
	}
}

//	攻撃ステートから出ていくときのメソッド
void AttackState<class T>::Finalize()
{
	if (owner_->GetAttackFlg())
	{
		//	攻撃が終わったとき攻撃権の破棄
		//	攻撃権をfalseに設定
		owner_->SetAttackFlg(false);
		//	エネミーからメタAIへMsgChangeAttackRightを送信する
		Meta::Instance().SendMessaging(owner_->GetId(), (int)Meta::Identity::Meta, MESSAGE_TYPE::MsgChangeAttackRight);
	}
}

//	コールドステートに入った時のメソッド
void CalledState<class T>::Initialize()
{
	owner_->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::WalkBWD), true);
	owner_->SetStateTimer(5.0f);
}

//	コールドステートで実行するメソッド
void CalledState<class T>::Update(float elapsedTime)
{
	//	タイマー管理
	float timer = owner_->GetStateTimer();
	timer -= elapsedTime;
	owner_->SetStateTimer(timer);

	if (timer < 0.0f)
	{
		//	徘徊ステートへ遷移
		owner_->GetStateMachine()->ChangeState(static_cast<int>(EnemySlime::State::Search));
	}
	//	対象をプレイヤー地点に設定
	owner_->SetTargetPosition(Player::Instance().GetTransform()->GetPosition());
	owner_->MoveToTarget(elapsedTime, 1.0f);
}

//	コールドステートから出ていくときのメソッド
void CalledState<class T>::Finalize()
{

}

//	戦闘待機ステートに入った時のメソッド
void StandbyState<class T>::Initialize()
{
	owner_->GetModel()->PlayAnimation(static_cast<int>(EnemyAnimation::IdleBattle), false);
}

//	戦闘待機ステートで実行するメソッド
void StandbyState<class T>::Update(float elapsedTime)
{
	//	攻撃権があるとき
	if (owner_->GetAttackFlg())
	{
		//	ステート変更
		owner-_>GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Battle::Attack));
	}
	//	目標地点をプレイヤー位置に設定
	owner_->SetTargetPosition(Player::Instance().GetTransform()->GetPosition());
	float vx = owner_->GetTargetPosition().x - owner_->GetPosition().x;
	float vy = owner_->GetTargetPosition().y - owner_->GetPosition().y;
	float vz = owner_->GetTargetPosition().z - owner_->GetPosition().z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);
	//	攻撃範囲から出たら追跡ステートへ遷移
	if (dist > owner_->GetAttackRange())
	{
		owner_->GetStateMachine()->ChangeSubState(static_cast<int>(EnemySlime::Battle::Pursuit));
	}
}

//	戦闘待機ステートから出ていくときのメソッド
void StandbyState<class T>::Finalize()
{

}
#endif