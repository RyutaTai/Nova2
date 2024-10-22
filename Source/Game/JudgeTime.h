#pragma once

#include <float.h>

class JudgeTime
{
public:
	JudgeTime();
	~JudgeTime(){}

	bool IsJudgeFlag(const float& time);	//	���Ԃ�minJudgeTime_��maxJudgeTime_�̊Ԃɂ��邩���f

	void SetMinJudgeTime(const float& minTime) { minJudgeTime_ = minTime; }		//	���莞�Ԃ̉����l�ݒ�
	void SetMaxJudgeTime(const float& maxTime) { maxJudgeTime_ = maxTime; }		//	���莞�Ԃ̏���l�ݒ�

private:
	bool judgeFlag_ = false;		//	����t���O
	float minJudgeTime_ = 0.0f;		//	���肷�鎞�Ԃ̉����l
	float maxJudgeTime_ = FLT_MAX;	//	���肷�鎞�Ԃ̏���l

};

