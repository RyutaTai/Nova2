#pragma once
#if 0
#include <vector>
#include <stack>
#include <map>

#include "BehaviorTree.h"

class NodeBase;
class EnemyBlueSlime;

//	Behavior保存データ
class BehaviorData
{
public:
	//	コンストラクタ
	BehaviorData() { Init(); }
	//	初期化
	void Init();
	//	シーケンスノードのプッシュ
	void PushSequenceNode(NodeBase* node) { sequenceStack.push(node); }
	//	シーケンスノードのポップ
	NodeBase* PopSequenceNode();
	//	シーケンスステップのゲッター
	int GetSequenceStep(std::string name);
	//	シーケンスステップのセッター
	void SetSequenceStep(std::string name, int step);
private:
	std::stack	<NodeBase*>			sequenceStack;			//	実行する中間ノードをスタック
	std::map	<std::string, int>	runSequenceStepMap;		//	実行中の中間ノードのステップを記録

};
#endif