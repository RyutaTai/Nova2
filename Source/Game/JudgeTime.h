#pragma once

#include <float.h>

class JudgeTime
{
public:
	JudgeTime();
	~JudgeTime(){}

	bool IsJudgeFlag(const float& time);	//	時間がminJudgeTime_とmaxJudgeTime_の間にあるか判断

	void SetMinJudgeTime(const float& minTime) { minJudgeTime_ = minTime; }		//	判定時間の下限値設定
	void SetMaxJudgeTime(const float& maxTime) { maxJudgeTime_ = maxTime; }		//	判定時間の上限値設定

private:
	bool judgeFlag_ = false;		//	判定フラグ
	float minJudgeTime_ = 0.0f;		//	判定する時間の下限値
	float maxJudgeTime_ = FLT_MAX;	//	判定する時間の上限値

};

