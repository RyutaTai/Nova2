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
	float						fps_;					//	���݂�fps�l��ێ�����ϐ�
	std::vector<float>			fpsBuffer_ = {};		//	fps���ۑ��p
	const size_t				maxHistorySize_ = 100;	//	fps��ۑ�����ő吔

	Graphics					graphics_;
	Input						input_;

};

