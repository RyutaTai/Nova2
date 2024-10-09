#pragma once
#if  0


#include <string>

class ActionBase;
class JudgmentBase;
class NodeBase;
class BehaviorData;
class Enemy;

//	ビヘイビアツリー
class BehaviorTree
{
public:
	//	選択ルール
	enum class SelectRule
	{
		Non,				//	無い末端ノード用
		Priority,			//	優先順位
		Sequence,			//	シーケンス
		SequentialLooping,	//	シーケンシャルルーピング
		Random,				//	ランダム
	};

public:
	BehaviorTree() :root(nullptr) , owner(nullptr) {}
	BehaviorTree(Enemy* enemy) :root(nullptr) , owner(enemy){}
	~BehaviorTree();

	//	実行ノードを推論する
	NodeBase* ActiveNodeInference(BehaviorData* data);

	//	シーケンスノードから推論開始
	NodeBase* SequenceBack(NodeBase* sequenceNode, BehaviorData* data);

	//	ノード追加
	//	parentName : 親ノードの名前
	//	entryname  : 自分の名前
	//	priority   : 優先順位を設定する。値が低いほど優先順位が高くなる(例：1と5なら,1が優先される)
	//	selectRule : 中間ノードの選択ルールを指定する。
	//	judgement  : 判定クラスのオブジェクトを指定する。newで各判定クラスを登録する。
	//	　　　　　　 判定クラスがない末端ノードや、どのノードも選択されなかったときなどに実行するノードはnullptrを指定する。
	//	action     : 実際のノードの行動を行うオブジェクトを指定する。
	//				   末端ノードにのみ指定し、ルートノードや中間ノードの場合は行動が無いのでnullptrを指定する。
	void AddNode(std::string parentName, std::string entryName, int priority, SelectRule selectRule, JudgmentBase* judgment, ActionBase* action);

	//	実行
	NodeBase* Run(NodeBase* actionNode, BehaviorData* data,float elapsedTime);

private:
	//	ノード全削除
	void NodeAllClear(NodeBase* delNode);

private:
	//	ルートノード
	NodeBase* root;
	Enemy* owner;

};

#endif //  0