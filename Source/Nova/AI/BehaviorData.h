#pragma once

#include <vector>
#include <stack>
#include <map>

#include "BehaviorTree.h"

template<class T>
class NodeBase;

//	Behavior保存データ
template<class T>
class BehaviorData
{
public:
	//	コンストラクタ
	BehaviorData() { Init(); }
	//	初期化
	void Init();
	//	シーケンスノードのプッシュ
	void PushSequenceNode(NodeBase<T>* node) { sequenceStack.push(node); }
	//	シーケンスノードのポップ
	NodeBase<T>* PopSequenceNode();
	//	シーケンスステップのゲッター
	int GetSequenceStep(std::string name);
	//	シーケンスステップのセッター
	void SetSequenceStep(std::string name, int step);
private:
	std::stack	<NodeBase<T>*>		sequenceStack;			//	実行する中間ノードをスタック
	std::map	<std::string, int>	runSequenceStepMap;		//	実行中の中間ノードのステップを記録

};
