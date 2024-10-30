#include "JudgeTime.h"

JudgeTime::JudgeTime()
{
	minJudgeTime_ = 0.0f;
	maxJudgeTime_ = FLT_MAX;
}

//	time‚ª”»’èŽžŠÔ‚Ì”ÍˆÍ‚É“ü‚Á‚Ä‚¢‚é‚©
const bool JudgeTime::IsJudgeFlag(const float& time)const
{
	return (minJudgeTime_ < time) && (time < maxJudgeTime_);
}
