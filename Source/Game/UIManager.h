#pragma once

#include <vector>

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

	UI*		GetUI(int num);

private:
	std::vector<UI*> userInterfaces_ = {};

	bool allDrawFlag_ = false;

};

