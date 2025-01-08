#pragma once

#include <vector>
#include <set>

#include "UI.h"

class UIManager
{
public:
	//	UI‚ÌŽí—Þ
	enum class UIType
	{
		Health,
		Tempo,
		RhythmJudgment,
		Max
	};

public:
	UIManager(){}
	~UIManager(){}

	static UIManager& Instance()
	{
		static UIManager uiManager;
		return uiManager;
	}

	void	Initialize();
	void	Update(const float& elpasedTime);
	void	Render();
	void	Finalize();
	void	DrawDebug();

	void	Register(UI* ui);

	void	SetIsVisible(const bool& isVisible);

	UI*		GetUIFromType(const UIType& type);
	UI*		GetUIFromNum(const int& num);

private:
	std::vector<UI*> userInterfaces_;
	std::set<UI*>	generates_;
	bool allIsVisible_ = true;

};

