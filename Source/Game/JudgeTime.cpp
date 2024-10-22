#include "JudgeTime.h"

JudgeTime::JudgeTime()
{
	judgeFlag_ = false;
	minJudgeTime_ = 0.0f;
	maxJudgeTime_ = FLT_MAX;
}

//	timeが判定時間の範囲に入っているか
bool JudgeTime::IsJudgeFlag(const float& time)
{
	judgeFlag_ = (minJudgeTime_ < time) && (time < maxJudgeTime_);

	return judgeFlag_;
}
