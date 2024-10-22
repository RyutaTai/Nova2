#include "JudgeTime.h"

JudgeTime::JudgeTime()
{
	judgeFlag_ = false;
	minJudgeTime_ = 0.0f;
	maxJudgeTime_ = FLT_MAX;
}

//	time�����莞�Ԃ͈̔͂ɓ����Ă��邩
bool JudgeTime::IsJudgeFlag(const float& time)
{
	judgeFlag_ = (minJudgeTime_ < time) && (time < maxJudgeTime_);

	return judgeFlag_;
}
