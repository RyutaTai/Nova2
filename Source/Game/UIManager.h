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

	void	SetDrawFlag(const bool& drawFlag);

	UI*		GetUIFromNum(int num);
	UI*		GetUIFromName(const std::string& name);

private:
	std::vector<UI*> userInterfaces_;
	std::set<UI*>	generates_;
	bool allDrawFlag_ = true;

};

