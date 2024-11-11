#include "BehaviorData.h"

#include "NodeBase.h"

//	シーケンスノードのポップ
template<class T>
NodeBase<T>* BehaviorData<T>::PopSequenceNode()
{
	//	空ならNULL
	if (sequenceStack.empty() != 0)
	{
		return nullptr;
	}
	NodeBase* node = sequenceStack.top();
	if (node != nullptr)
	{
		//	取り出したデータを削除
		sequenceStack.pop();
	}
	return node;
}

//	シーケンスステップのゲッター
template<class T>
int BehaviorData<T>::GetSequenceStep(std::string name)
{
	if (runSequenceStepMap.count(name) == 0)
	{
		runSequenceStepMap.insert(std::make_pair(name,0));
	}

	return runSequenceStepMap.at(name);
}

//	シーケンスステップのセッター
template<class T>
void BehaviorData<T>::SetSequenceStep(std::string name, int step)
{
	runSequenceStepMap.at(name) = step;
}

//	初期化
template<class T>
void BehaviorData<T>::Init()
{
	runSequenceStepMap.clear();
	while (sequenceStack.size() > 0)
	{
		sequenceStack.pop();
	}
}
