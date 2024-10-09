#pragma once
#if  0


#include <string>

class ActionBase;
class JudgmentBase;
class NodeBase;
class BehaviorData;
class Enemy;

//	�r�w�C�r�A�c���[
class BehaviorTree
{
public:
	//	�I�����[��
	enum class SelectRule
	{
		Non,				//	�������[�m�[�h�p
		Priority,			//	�D�揇��
		Sequence,			//	�V�[�P���X
		SequentialLooping,	//	�V�[�P���V�������[�s���O
		Random,				//	�����_��
	};

public:
	BehaviorTree() :root(nullptr) , owner(nullptr) {}
	BehaviorTree(Enemy* enemy) :root(nullptr) , owner(enemy){}
	~BehaviorTree();

	//	���s�m�[�h�𐄘_����
	NodeBase* ActiveNodeInference(BehaviorData* data);

	//	�V�[�P���X�m�[�h���琄�_�J�n
	NodeBase* SequenceBack(NodeBase* sequenceNode, BehaviorData* data);

	//	�m�[�h�ǉ�
	//	parentName : �e�m�[�h�̖��O
	//	entryname  : �����̖��O
	//	priority   : �D�揇�ʂ�ݒ肷��B�l���Ⴂ�قǗD�揇�ʂ������Ȃ�(��F1��5�Ȃ�,1���D�悳���)
	//	selectRule : ���ԃm�[�h�̑I�����[�����w�肷��B
	//	judgement  : ����N���X�̃I�u�W�F�N�g���w�肷��Bnew�Ŋe����N���X��o�^����B
	//	�@�@�@�@�@�@ ����N���X���Ȃ����[�m�[�h��A�ǂ̃m�[�h���I������Ȃ������Ƃ��ȂǂɎ��s����m�[�h��nullptr���w�肷��B
	//	action     : ���ۂ̃m�[�h�̍s�����s���I�u�W�F�N�g���w�肷��B
	//				   ���[�m�[�h�ɂ̂ݎw�肵�A���[�g�m�[�h�⒆�ԃm�[�h�̏ꍇ�͍s���������̂�nullptr���w�肷��B
	void AddNode(std::string parentName, std::string entryName, int priority, SelectRule selectRule, JudgmentBase* judgment, ActionBase* action);

	//	���s
	NodeBase* Run(NodeBase* actionNode, BehaviorData* data,float elapsedTime);

private:
	//	�m�[�h�S�폜
	void NodeAllClear(NodeBase* delNode);

private:
	//	���[�g�m�[�h
	NodeBase* root;
	Enemy* owner;

};

#endif //  0