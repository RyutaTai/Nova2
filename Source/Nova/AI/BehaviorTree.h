#pragma once

#include <string>
#include <vector>
#include <stack>
#include <map>

//	前方宣言
template<class T>
class BehaviorData;

template<class T>
class ActionBase;

template<class T>
class JudgmentBase;

template<class T>
class NodeBase;

//	メモリリーク調査用
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

//	ビヘイビアツリー
template<class T>
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
	BehaviorTree() :root_(nullptr), owner_(nullptr) {}
	BehaviorTree(T* owner) :root_(nullptr), owner_(owner) {}
	~BehaviorTree();

	//	実行ノードを推論する
	NodeBase<T>* ActiveNodeInference(BehaviorData<T>* data);

	//	シーケンスノードから推論開始
	NodeBase<T>* SequenceBack(NodeBase<T>* sequenceNode, BehaviorData<T>* data);

	//	ノード追加
	//	parentName : 親ノードの名前
	//	entryname  : 自分の名前
	//	priority   : 優先順位を設定する。値が低いほど優先順位が高くなる(例：1と5なら,1が優先される)
	//	selectRule : 中間ノードの選択ルールを指定する。
	//	judgement  : 判定クラスのオブジェクトを指定する。newで各判定クラスを登録する。
	//	　　　　　　 判定クラスがない末端ノードや、どのノードも選択されなかったときなどに実行するノードはnullptrを指定する。
	//	action     : 実際のノードの行動を行うオブジェクトを指定する。
	//				   末端ノードにのみ指定し、ルートノードや中間ノードの場合は行動が無いのでnullptrを指定する。
	void AddNode(std::string parentName, std::string entryName, const int& priority, SelectRule selectRule, JudgmentBase<T>* judgment, ActionBase<T>* action);

	//	実行
	NodeBase<T>* Run(NodeBase<T>* actionNode, BehaviorData<T>* data, const float& elapsedTime);

private:
	//	ノード全削除
	void NodeAllClear(NodeBase<T>* delNode);

private:
	//	ルートノード
	NodeBase<T>* root_;
	T* owner_;

};

//	Behaviorデータ保存
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

//	ノード基底クラス
template<class T>
class NodeBase
{
public:
	//	コンストラクタ
	NodeBase(std::string name, NodeBase<T>* parent, NodeBase<T>* sibling, int priority,
		BehaviorTree<T>::SelectRule selectRule, JudgmentBase<T>* judgment, ActionBase<T>* action, int hierarchyNo) :
		name(name), parent(parent), sibling(sibling), priority(priority),
		selectRule(selectRule), judgment(judgment), action(action), hierarchyNo(hierarchyNo),
		children(NULL)
	{
	}
	//	デストラクタ
	~NodeBase();
	//	名前ゲッター
	std::string GetName() { return name; }
	//	親ノードゲッター
	NodeBase<T>* GetParent() { return parent; }
	//	子ノードゲッター
	NodeBase<T>* GetChild(int index);
	//	子ノードゲッター(末尾)
	NodeBase<T>* GetLastChild();
	//	子ノードゲッター(先頭)
	NodeBase<T>* GetTopChild();
	//	兄弟ノードゲッター
	NodeBase<T>* GetSibling() { return sibling; }
	//	階層番号ゲッター
	int GetHirerchyNo() { return hierarchyNo; }
	//	優先順位ゲッター
	int GetPriority() { return priority; }
	//	親ノードセッター
	void SetParent(NodeBase<T>* parent) { this->parent = parent; }
	//	子ノード追加
	void AddChild(NodeBase<T>* child) { children.push_back(child); }
	//	兄弟ノードセッター
	void SetSibling(NodeBase<T>* sibling) { this->sibling = sibling; }
	//	行動データを持っているか
	bool HasAction() { return action != nullptr ? true : false; }
	//	実行可否判定
	bool Judgment();
	//	優先順位選択
	NodeBase<T>* SelectPriority(std::vector <NodeBase<T>*>* list);
	//	ランダム選択
	NodeBase<T>* SelectRandom(std::vector <NodeBase<T>*>* list);
	//	シーケンス選択
	NodeBase<T>* SelectSequence(std::vector <NodeBase<T>*>* list, BehaviorData<T>* data);
	//	ノード検索
	NodeBase<T>* SearchNode(std::string searchName);
	//	ノード推論
	NodeBase<T>* Inference(BehaviorData<T>* data);
	//	実行
	ActionBase<T>::State Run(float elapsedTime);
	std::vector	<NodeBase<T>*>	children;		//	子ノード

protected:
	std::string					name;			//	名前
	BehaviorTree<T>::SelectRule	selectRule;		//	選択ルール
	JudgmentBase<T>*			judgment;		//	判定クラス
	ActionBase<T>*				action;			//	実行クラス
	unsigned int				priority;		//	優先順位
	NodeBase<T>*				parent;			//	親ノード
	NodeBase<T>*				sibling;		//	兄弟ノード
	int							hierarchyNo;	//	階層番号
};

//	行動処理基底クラス
template<class T>
class ActionBase
{
public:
	ActionBase(T* owner) :owner_(owner) {}
	//	実行情報
	enum class State
	{
		Run,		//	実行中
		Failed,		//	実行失敗
		Complete,	//	実行成功
	};

	virtual ActionBase<T>::State Run(const float& elapsedTime) = 0;	//	実行処理

protected:
	T* owner_;
	int step = 0;
};

//	実行判定基底クラス
template<class T>
class JudgmentBase
{
public:
	JudgmentBase(T* owner) :owner_(owner) {}
	virtual bool Judgment() = 0;

protected:
	T* owner_;

};