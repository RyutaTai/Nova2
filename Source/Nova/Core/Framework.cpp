#include "Framework.h"

#include <dxgi.h>

#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../Graphics/ShadowMap.h"
#include "../Scenes/SceneManager.h"
#include "../Scenes/SceneTitle.h"
#include "../Scenes/SceneGame.h"
#include "../Scenes/SceneDemo.h"
#include "../Debug/SceneModelCheck.h"
#include "../Resources/EffectManager.h"
#include "../Audio/AudioManager.h"

HighResolutionTimer Framework::tictoc_ = {};

//	�R���X�g���N�^
Framework::Framework(HWND hwnd)
	: graphics_(hwnd, FULLSCREEN/*fullscreen*/),
	input_(hwnd)
{
	//	XAUDIO2�f�o�b�O�p
	{
		HRESULT hr = S_OK;
		IXAudio2* xaudio = nullptr;

		hr = XAudio2Create(&xaudio, 0, XAUDIO2_DEFAULT_PROCESSOR);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// create masteringvoice
		IXAudio2MasteringVoice* masteringVoice = nullptr;
#if 0
		hr = xaudio->CreateMasteringVoice(&masteringVoice, 2, 44100/*�T���v�����O���[�g*/, 0U, NULL, 0, AudioCategory_GameEffects);
#else
		hr = xaudio->CreateMasteringVoice(&masteringVoice, XAUDIO2_DEFAULT_CHANNELS, 44100/*�T���v�����O���[�g*/, 0U, NULL, 0, AudioCategory_GameEffects);
#endif
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		XAUDIO2_VOICE_DETAILS masterDetails;
		masteringVoice->GetVoiceDetails(&masterDetails);

		//	�}�X�^�����O�{�C�X���(�f�o�b�O�p)
		int masterInputChannel = masterDetails.InputChannels;
		DWORD ChannelMask = {};
		masteringVoice->GetChannelMask(&ChannelMask);
		int masterSampleRate = masterDetails.InputSampleRate;
	}

}

//	�f�X�g���N�^
Framework::~Framework()
{
	////	�V�[���I����
	//SceneManager::Instance().Clear();

	////	�G�t�F�N�g�}�l�[�W���[�I����
	//EffectManager::Instance().Finalize();
}

//	������
bool Framework::Initialize()
{
	// TODO:�V�[���������؂�ւ�
	SceneManager::Instance().ChangeScene(new SceneTitle());
	//SceneManager::Instance().ChangeScene(new SceneGame());
	//SceneManager::Instance().ChangeScene(new SceneDemo());
	//SceneManager::Instance().ChangeScene(new SceneModelCheck());

	//	�G�t�F�N�g�}�l�[�W���[������
	EffectManager::Instance().Initialize();

	return true;
}

//	�X�V����
void Framework::Update(const float& elapsedTime/*Elapsed seconds from last frame*/)
{
	IMGUI_CTRL_CLEAR_FRAME();

	//	Input������
	input_.Update();

	//	�I�[�f�B�I�X�V����
	AudioManager::Instance().Update(elapsedTime);

	//	�V�[���̍X�V
	SceneManager::Instance().Update(elapsedTime);

}

//	�A�v���P�[�V�������[�v
int Framework::Run()
{
	MSG msg{};

	if (!Initialize())
	{
		return 0;
	}

	IMGUI_CTRL_INITIALIZE(graphics_.GetHwnd(), graphics_.GetDevice(), graphics_.GetDeviceContext());

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			tictoc_.Tick();
			CalculateFrameStats();
			Update(tictoc_.GetDeltaTime());
#if _DEBUG
			DrawDebug();	//	Update()��Render()�̊ԂŌĂԁB(ImGui::NewFrame();��ImGui::Render();�ŋ��ޕK�v�����邪�A�����Update��Render�ŌĂ�ł��邽�߁B)
#endif
			Render();
		}
	}

	IMGUI_CTRL_UNINITIALIZE();

#if 0
	BOOL fullscreen = 0;
	graphics_.GetSwapChain()->GetFullscreenState(&fullscreen, 0);
	if (fullscreen)
	{
		graphics_.GetSwapChain()->SetFullscreenState(FALSE, 0);
	}
#endif

	return Uninitialize() ? static_cast<int>(msg.wParam) : 0;
}

//	�t���[�����[�g�v�Z
void Framework::CalculateFrameStats()
{
	if (++framesPerSecond_, (tictoc_.TimeStamp() - elapsedTime_) >= 1.0f)
	{
		fps_ = static_cast<float>(framesPerSecond_);
		std::wostringstream outs;
		outs.precision(6);
		outs << APPLICATION_NAME << L" : FPS : " << fps_ << L" / " << L"Frame Time : " << 1000.0f / fps_ << L" (ms)";
		SetWindowTextW(graphics_.GetHwnd(), outs.str().c_str());

		//	FPS�l���o�b�t�@�ɒǉ�
		if (fpsBuffer_.size() >= maxHistorySize_)
		{
			fpsBuffer_.erase(fpsBuffer_.begin());
		}
		fpsBuffer_.push_back(fps_);

		framesPerSecond_ = 0;
		elapsedTime_ += 1.0f;
	}
}

//	���b�Z�[�W�n���h��
LRESULT CALLBACK Framework::HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
#ifdef USE_IMGUI
	IMGUI_CTRL_WND_PRC_HANDLER(hwnd, msg, wparam, lparam);
#endif
	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps{};
		BeginPaint(hwnd, &ps);

		EndPaint(hwnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE)	//	Esc�L�[�ŗ��Ƃ�
		{
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
		break;
	case WM_ENTERSIZEMOVE:
		tictoc_.Stop();
		break;
	case WM_EXITSIZEMOVE:
		tictoc_.Start();
		break;
	case WM_SIZE:
	{
#if 1
		RECT client_rect{};
		GetClientRect(hwnd, &client_rect);
		graphics_.OnSizeChanged(static_cast<UINT64>(client_rect.right - client_rect.left), client_rect.bottom - client_rect.top);
#endif
		break;
	}
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

//	�`�揈��
void Framework::Render()
{
	//	�ʃX���b�h���Ƀf�o�C�X�R���e�L�X�g���g���Ă����ꍇ��
	//	�����A�N�Z�X���Ȃ��悤�ɔr�����䂷��
	std::lock_guard<std::mutex>lock(graphics_.GetMutex());

	//	�T���v���[�X�e�[�g�ݒ�
	graphics_.GetShader()->SetSamplerState(graphics_.GetDeviceContext());

	//	ShadowMap����
	//	Activate
	ShadowMap::Instance().Activate();
	//	ShadowRender
	SceneManager::Instance().ShadowRender();
	//	Deactivate
	ShadowMap::Instance().Deactivate();

	//	Scene�`��
	FLOAT color[]{ 1, 0, 0, 1 };
	ID3D11RenderTargetView* renderTargetView = graphics_.GetRenderTargetView();
	graphics_.GetDeviceContext()->ClearRenderTargetView(graphics_.GetRenderTargetView(), color);
	graphics_.GetDeviceContext()->ClearDepthStencilView(graphics_.GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	graphics_.GetDeviceContext()->OMSetRenderTargets(1, &renderTargetView, graphics_.GetDepthStencilView());

	SceneManager::Instance().Render();

	//	ImGui�`��
	IMGUI_CTRL_DISPLAY();

	// ���s
	graphics_.PresentFrame();

	//	������ȉ��͕`�悳��Ȃ�
}

//	�I����
bool Framework::Uninitialize()
{
	//	�V�[���I����
	SceneManager::Instance().Clear();

	//	�G�t�F�N�g�}�l�[�W���[�I����
	EffectManager::Instance().Finalize();


	return true;
}

//	�f�o�b�O�`��
void Framework::DrawDebug()
{
	//	�f�o�b�O�E�B���h�E���쐬
	ImGui::Begin("Framework");

	//	fps�̃O���t��`��
	ImGui::PlotLines("FPS Graph", fpsBuffer_.data(), static_cast<int>(fpsBuffer_.size()), 0, nullptr, 0.0f, FLT_MAX, ImVec2(0, 80));
	// ���݂�FPS�𐔒l�Ƃ��ĕ\��
	ImGui::Text("Current FPS: %.2f", fps_);
	
	graphics_.DrawDebug();

	ImGui::End();
}