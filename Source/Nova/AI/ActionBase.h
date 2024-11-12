#pragma once

class Dragonkin;

//	行動処理基底クラス
class ActionBase
{
public:
	ActionBase(Dragonkin* enemy):owner_(enemy){}
	//	実行情報
	enum class State
	{
		Run,		//	実行中
		Failed,		//	実行失敗
		Complete,	//	実行成功
	};

	virtual ActionBase::State Run(const float& elapsedTime) = 0;	//	実行処理
	virtual void DrawDebug() = 0;									//	デバッグ描画

protected:
	Dragonkin* owner_ = nullptr;
	int step_ = 0;

};
