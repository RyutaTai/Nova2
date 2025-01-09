#pragma once

#include <vector>
#include <set>

#include "UI.h"

class UIManager
{
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
	void	Remove(UI* ui);

	void	SetIsVisible(const bool& isVisible);

	UI*		GetUIFromNum(const int& num);

private:
	std::vector<UI*> userInterfaces_;
	std::set<UI*>	generates_;
	std::set<UI*>	removes_;
	bool allIsVisible_ = true;

};

