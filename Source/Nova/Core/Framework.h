#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <d3d11.h>
#include <wrl.h>

#include <DirectXMath.h>

#include "../Others/Misc.h"
#include "../Core/HighResolutionTimer.h"
#include "../Graphics/Graphics.h"
#include "../../Imgui/ImGuiCtrl.h"
#include "../Input/Input.h"
#include "../Audio/Audio.h"

CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };
CONST BOOL FULLSCREEN{ FALSE };
CONST LPCWSTR APPLICATION_NAME{ L"Groove" };

class Framework
{
public:
	Framework(HWND hwnd);
	~Framework();

	Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework(Framework&&) noexcept = delete;
	Framework& operator=(Framework&&) noexcept = delete;

	int Run();

	LRESULT CALLBACK	HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static float		GetDeltaTime() { return tictoc_.GetDeltaTime(); }

private:
	bool	Initialize();
	void	Update(const float& elapsedTime/*Elapsed seconds from last frame*/);
	void	Render();
	bool	Uninitialize();
	void	DrawDebug();

	void	CalculateFrameStats();

private:
    static HighResolutionTimer	tictoc_;
	float						elapsedTime_ = 0.0f;

	uint32_t					framesPerSecond_ = 0;
	float						fps_;					//	現在のfps値を保持する変数
	std::vector<float>			fpsBuffer_ = {};		//	fps数保存用
	const size_t				maxHistorySize_ = 100;	//	fpsを保存する最大数

	Graphics					graphics_;
	Input						input_;

};

