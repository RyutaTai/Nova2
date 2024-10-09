#include "State.h"

////	2層目ステートセット
//void HierarchicalState<class T>::SetSubState(int newState)
//{
//	subState = subStatePool.at(newState);
//	subState->Initialize();
//}
//
////	2層目のステート切り替え
//void HierarchicalState<class T>::ChangeSubState(int newState)
//{
//	//	現在のステートがあれば、Exit関数を実行
//	//	次に新しいステートをセット、新しいステートのEnter関数を呼び出す。
//	if (subState)subState->Finalize();
//
//	SetSubState(newState);
//	subState->Initialize();
//}
//
////	サブステート登録（state を subStatePool に登録）
//void HierarchicalState<class T>::RegisterSubState(State<T>* state)
//{
//	subStatePool.emplace_back(state);
//}
//
////	サブステートの番号取得
//int HierarchicalState<class T>::GetSubStateIndex()
//{
//	int i = 0;
//	for (State<T>* state : subStatePool)
//	{
//		if (subState == state)return i;
//		++i;
//	}
//	return i;
//}
