#include "State.h"

////	2�w�ڃX�e�[�g�Z�b�g
//void HierarchicalState<class T>::SetSubState(int newState)
//{
//	subState = subStatePool.at(newState);
//	subState->Initialize();
//}
//
////	2�w�ڂ̃X�e�[�g�؂�ւ�
//void HierarchicalState<class T>::ChangeSubState(int newState)
//{
//	//	���݂̃X�e�[�g������΁AExit�֐������s
//	//	���ɐV�����X�e�[�g���Z�b�g�A�V�����X�e�[�g��Enter�֐����Ăяo���B
//	if (subState)subState->Finalize();
//
//	SetSubState(newState);
//	subState->Initialize();
//}
//
////	�T�u�X�e�[�g�o�^�istate �� subStatePool �ɓo�^�j
//void HierarchicalState<class T>::RegisterSubState(State<T>* state)
//{
//	subStatePool.emplace_back(state);
//}
//
////	�T�u�X�e�[�g�̔ԍ��擾
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
