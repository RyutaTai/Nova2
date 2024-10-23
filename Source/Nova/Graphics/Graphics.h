#pragma once

#include <wrl.h>
#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>
#include <mutex>
#include <dxgi1_6.h>

#ifdef ENABLE_DIRECT2D
#include <d2d1_1.h>
#include <dwrite.h>
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#endif

#include "../Graphics/Framebuffer.h"
#include "../Graphics/FullScreenQuad.h"
#include "../Graphics/Shader.h"
#include "../Debug/DebugRenderer.h"
#include "ShadowMap.h"

class Graphics
{
public:	//	�\����
	struct SceneConstants
	{
		DirectX::XMFLOAT4X4 viewProjection_;
		DirectX::XMFLOAT4   lightDirection_;
		DirectX::XMFLOAT4   cameraPosition_;
		DirectX::XMFLOAT4X4 lightViewProjection_;	//	Shadow
		DirectX::XMFLOAT4X4 invViewProjection_;		//	Skymap
	};

public:
	Graphics(HWND hwnd, bool fullscreen);
	~Graphics() {}

	static Graphics& Instance()
	{
		return *instance_;
	}
	void DrawDebug();


	void	AcquireHighPerformanceAdapter(IDXGIFactory6* dxgiFactory6, IDXGIAdapter3** dxgiAdapter3);
	void	CreateSwapChain(IDXGIFactory6* dxgiFactory6);
	void	OnSizeChanged(UINT64 width, UINT height);
	void	StylizeWindow(bool fullscreen);
	void	PresentFrame();
	size_t	VideoMemoryUsage();
#ifdef ENABLE_DIRECT2D
	void	CreateDirect2dObjects();
#endif

	//	�Z�b�^�[
	void ClearSceneConstant() { sceneConstant_ = {}; }
	void SetSceneConstant(const SceneConstants& sceneConstant)					{ sceneConstant_ = sceneConstant; }
	void SetViewProjection(const DirectX::XMFLOAT4X4& viewProjection)			{ sceneConstant_.viewProjection_ = viewProjection; }
	void SetViewProjection(const DirectX::XMMATRIX& viewProjection)				{ DirectX::XMStoreFloat4x4(&sceneConstant_.viewProjection_, viewProjection); }
	void SetLightDirection(const DirectX::XMFLOAT4& lightDirection)				{ sceneConstant_.lightDirection_ = lightDirection; }
	void SetCameraPosition(const DirectX::XMFLOAT4& cameraPosition)				{ sceneConstant_.cameraPosition_ = cameraPosition; }
	void SetLightViewProjection(const DirectX::XMFLOAT4X4& lightViewProjection) { sceneConstant_.lightViewProjection_ = lightViewProjection; }
	void SetInvViewProjection(const DirectX::XMFLOAT4X4& invViewProjection)		{ sceneConstant_.invViewProjection_ = invViewProjection; }
	void SetInvViewProjection(const DirectX::XMMATRIX& invViewProjection)		{ DirectX::XMStoreFloat4x4(&sceneConstant_.invViewProjection_, invViewProjection); }
	void SetIsVSync(bool isVSync);

	//	�Q�b�^�[
	CONST HWND					GetHwnd()					CONST	{ return hwnd_; }
	ID3D11Device*				GetDevice()							{ return device_.Get(); }
	ID3D11DeviceContext*		GetDeviceContext()					{ return deviceContext_.Get(); }
	IDXGISwapChain1*			GetSwapChain()						{ return swapChain_.Get(); }
	ID3D11RenderTargetView*		GetRenderTargetView()				{ return renderTargetView_.Get(); }
	ID3D11DepthStencilView*		GetDepthStencilView()				{ return depthStencilView_.Get(); }
	ID3D11Buffer*				GetConstantBuffer()					{ return constantBuffer_.Get(); }
	Shader*						GetShader()							{ return shader_.get(); }
	const SceneConstants		GetSceneConstant()			const	{ return sceneConstant_; }
	const DirectX::XMFLOAT4X4	GetViewProjection()			const	{ return sceneConstant_.viewProjection_; }
	const DirectX::XMFLOAT4		GetLightDirection()			const	{ return sceneConstant_.lightDirection_; }
	const DirectX::XMFLOAT4		GetCameraPosition()			const	{ return sceneConstant_.cameraPosition_; }
	const DirectX::XMFLOAT4X4	GetLightViewProjection()	const	{ return sceneConstant_.lightViewProjection_; }
	const DirectX::XMFLOAT4X4	GetInvViewProjection()		const	{ return sceneConstant_.invViewProjection_; }
	DebugRenderer*				GetDebugRenderer()					{ return debugRenderer_.get(); }
	std::mutex&					GetMutex()							{ return mutex_; }			//�~���[�e�b�N�X�擾

private:
	static Graphics*								instance_;
	std::unique_ptr<Shader>							shader_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Device>			device_;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		deviceContext_;
	Microsoft::WRL::ComPtr<IDXGISwapChain1>			swapChain_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView_;

	//	SceneConstant
	SceneConstants	sceneConstant_;

	//	ConstantBuffer
	Microsoft::WRL::ComPtr<ID3D11Buffer>	constantBuffer_;
	std::unique_ptr<FrameBuffer>			frameBuffers_[8];	//	�g���Ă��Ȃ�
	std::unique_ptr<FullScreenQuad>			bitBlockTransfer_;

	//	��������
	bool	isVSync_ = true;			//	���������t���O
	UINT	vSyncInterval_ = 1;			//	���������Ԋu(0�Ȃ琂�������I�t�A1�Ȃ�I��)

	//	FullScreen
	CONST HWND	hwnd_;
	Microsoft::WRL::ComPtr<IDXGIAdapter3>			adapter_;
#ifdef ENABLE_DIRECT2D
	Microsoft::WRL::ComPtr<ID2D1DeviceContext>		d2d1DeviceContext_;
	Microsoft::WRL::ComPtr<IDWriteTextFormat>		dwriteTextFormats_[8];
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	d2dSolidColorBrushes_[8];
#endif
	bool	fullScreenMode_ = false;
	bool	tearingSupported_ = false;
	RECT	windowedRect_;
	DWORD	windowedStyle_;
	SIZE	frameBufferDimensions_;

private:
	//	�f�o�b�O�v���~�e�B�u
	std::unique_ptr<DebugRenderer>	debugRenderer_;	//	�f�o�b�O�v���~�e�B�u�`��p
	std::mutex						mutex_;

};

