#include "JudgeTime.h"

JudgeTime::JudgeTime()
{
	judgeFlag_ = false;
	minJudgeTime_ = 0.0f;
	maxJudgeTime_ = FLT_MAX;
}

//	time‚ª”»’èŽžŠÔ‚Ì”ÍˆÍ‚É“ü‚Á‚Ä‚¢‚é‚©
bool JudgeTime::IsJudgeFlag(const float& time)
{
	judgeFlag_ = (minJudgeTime_ < time) && (time < maxJudgeTime_);

	return judgeFlag_;
}
