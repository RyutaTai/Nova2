#pragma once

#include <vector>
#include <stack>
#include <map>

#include "BehaviorTree.h"

class NodeBase;
class Enemy;

//	Behavior保存データ
class BehaviorData
{
public:
	//	コンストラクタ
	BehaviorData() { Initialize(); }
	//	シーケンスノードのプッシュ
	void PushSequenceNode(NodeBase* node) { sequenceStack_.push(node); }
	//	シーケンスノードのポップ
	NodeBase* PopSequenceNode();
	//	シーケンスステップのゲッター
	int GetSequenceStep(std::string name);
	//	シーケンスステップのセッター
	void SetSequenceStep(std::string name, int step);
	// 初期化
	void Initialize();

private:
	std::stack<NodeBase*>		sequenceStack_;			//	実行する中間ノードをスタック
	std::map<std::string, int>	runSequenceStepMap_;	//	実行中の中間ノードのステップを記録]

};
