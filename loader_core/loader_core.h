#pragma once
#include "stdafx.h"

#define LOG_INFO(m, t, ...) loader_core::instance.log_text_fmt(LL_INFO, m, t, __VA_ARGS__)
#define LOG_ERROR(m, t, ...) loader_core::instance.log_text_fmt(LL_ERR, m, t, __VA_ARGS__)
#define LOG_WARNING(m, t, ...) loader_core::instance.log_text_fmt(LL_WARN, m, t, __VA_ARGS__)
#define LOG_DEBUG(m, t, ...) loader_core::instance.log_text_fmt(LL_DEBUG, m, t, __VA_ARGS__)

typedef enum loader_state {
	LDR_DLL_LOADED,	
	LDR_ADDON_LOAD,
	LDR_INGAME,
	LDR_UNLOAD,
	LDR_UNLOADED
} loader_state;

#define DX11_CREATE_FDEF IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext
#define DX11_CREATE_PLIST pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext

class loader_core {

public:
	loader_core();
	~loader_core();
	
	IDirect3D9* RouteD3DCreate(UINT sdkVer);
	IDirect3D9* OnD3DCreate(UINT sdkVer);
	HRESULT RouteD3D11CreateDeviceAndSwapChain(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext);
	HRESULT OnD3D11CreateDeviceAndSwapChain(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext);
	HRESULT RouteDXGIFactoryCreate(UINT ver, UINT Flags, REFIID riid, void** ppFactory);
	HRESULT OnDXGIFactoryCreate(UINT ver, UINT Flags, REFIID riid, void** ppFactory);
	void SignalUnload();
	
	loader_state GetCurrentState();

	void LoadAddonsFromDir(const wchar_t* dir);

	void log_text_fmt(gw2al_log_level level, const wchar_t * source, const wchar_t * fmt, ...);

	static loader_core instance;

private:	
	void innerInit();
	BOOL SwitchState(loader_state newState);
	loader_state state;

};
