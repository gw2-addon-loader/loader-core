#include "stdafx.h"
#include "build_version.h"

loader_core loader_core::instance;

loader_core::loader_core()
{
	state = LDR_DLL_LOADED;
}

loader_core::~loader_core()
{
}

loader_state loader_core::GetCurrentState()
{
	return state;
}

void loader_core::LoadAddonsFromDir(const wchar_t * dir)
{
	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;

	wchar_t sPath[2048];
	wsprintf(sPath, L"%s\\*.*", dir);

	LOG_INFO(L"core", L"Loading addons from \"./%s\" path", dir);

	if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do
	{
		if (wcscmp(fdFile.cFileName, L".") != 0
			&& wcscmp(fdFile.cFileName, L"..") != 0)
		{
			if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//if addon loaded as dependency, do not try to load it again
				if (gw2al_core__query_addon(gw2al_core__hash_name(fdFile.cFileName)))
					continue;

				LOG_INFO(L"core", L"Loading %s", fdFile.cFileName);

				gw2al_api_ret ret = gw2al_core__load_addon(fdFile.cFileName);

				switch (ret)
				{
					case GW2AL_BAD_DLL:
						LOG_ERROR(L"core", L"%s/%s/gw2addon_%s.dll missing one of export functions", dir, fdFile.cFileName, fdFile.cFileName);
						break;
					case GW2AL_NOT_FOUND:
						LOG_WARNING(L"core", L"%s/%s/gw2addon_%s.dll not found", dir, fdFile.cFileName, fdFile.cFileName);
						break;
					case GW2AL_DEP_NOT_LOADED:
						LOG_ERROR(L"core", L"%s/%s/gw2addon_%s.dll dependency missing", dir, fdFile.cFileName, fdFile.cFileName);
						break;
					case GW2AL_DEP_OUTDATED:
						LOG_ERROR(L"core", L"%s/%s/gw2addon_%s.dll dependency outdated", dir, fdFile.cFileName, fdFile.cFileName);
						break;
					case GW2AL_OK:
						;// LOG_INFO(L"core", L"%s/%s/gw2addon_%s.dll loaded", dir, fdFile.cFileName, fdFile.cFileName);
						break;
					default:
						LOG_ERROR(L"core", L"Error %u loading %s/%s/gw2addon_%s.dll", ret, dir, fdFile.cFileName, fdFile.cFileName);
						break;
				}
			}
		}
	} while (FindNextFile(hFind, &fdFile));

	FindClose(hFind);

	LOG_INFO(L"core", L"Done loading addons", dir);
}

void loader_core::log_text_fmt(gw2al_log_level level, const wchar_t * source, const wchar_t * fmt, ...)
{
	static wchar_t buf[4096];

	va_list arg;
	va_start(arg, fmt);

	vswprintf(buf, 4096, fmt, arg);

	va_end(arg);

	gw2al_core__log_text(level, (wchar_t*)source, buf);
}

void loader_core::innerInit()
{
	if (SwitchState(LDR_ADDON_LOAD))
	{
		bool isFirstLoad = gw2al_core__init();

		LOG_INFO(L"core", L"Addon loader v%u.%u (%S) %S", LOADER_CORE_VER_MAJOR, LOADER_CORE_VER_MINOR, LOADER_CORE_VER_NAME,
			isFirstLoad ? "initialized" : "reinit");

		LoadAddonsFromDir(L"addons");

		SwitchState(LDR_INGAME);
	}
}

IDirect3D9* loader_core::RouteD3DCreate(UINT sdkVer)
{
	IDirect3D9* (*d3d9_create_hook)() = (IDirect3D9 * (*)())gw2al_core__query_function(GW2AL_CORE_FUNN_D3DCREATE_HOOK);

	IDirect3D9* ret = NULL;

	if (d3d9_create_hook)
	{
		LOG_DEBUG(L"core", L"Calling D3D9Create, hook = 0x%016llX", d3d9_create_hook);
		ret = d3d9_create_hook();
	}
	else
	{
		LOG_DEBUG(L"core", L"Loading system d3d9.dll");

		wchar_t infoBuf[4096];
		GetSystemDirectory(infoBuf, 4096);
		lstrcatW(infoBuf, L"\\d3d9.dll");

		HMODULE sys_d3d9 = LoadLibrary(infoBuf);

		typedef IDirect3D9* (WINAPI* Direct3DCreate9Func)(UINT sdkver);

		Direct3DCreate9Func origDirect3DCreate9 = (Direct3DCreate9Func)GetProcAddress(sys_d3d9, "Direct3DCreate9");
		ret = origDirect3DCreate9(sdkVer);
	}

	LOG_DEBUG(L"core", L"ID3D9 = 0x%016llX", ret);

	return ret;
}

HRESULT loader_core::RouteDXGIFactoryCreate(UINT ver, UINT Flags, REFIID riid, void** ppFactory)
{
	typedef HRESULT(WINAPI* DXGIFactoryCreate0)(REFIID riid, void** ppFactory);
	typedef HRESULT(WINAPI* DXGIFactoryCreate1)(REFIID riid, void** ppFactory);
	typedef HRESULT(WINAPI* DXGIFactoryCreate2)(UINT Flags, REFIID riid, void** ppFactory);
	typedef HRESULT(WINAPI* DXGIFactoryCreateHook)(UINT ver, UINT Flags, REFIID riid, void** ppFactory);

	DXGIFactoryCreateHook dxgi_create_hook = (DXGIFactoryCreateHook)gw2al_core__query_function(GW2AL_CORE_FUNN_DXGICREATE_HOOK);

	HRESULT ret = NULL;

	if (dxgi_create_hook)
	{
		LOG_DEBUG(L"core", L"Calling DXGICreate, hook = 0x%016llX", dxgi_create_hook);
		ret = dxgi_create_hook(ver, Flags, riid, ppFactory);
	}
	else
	{
		LOG_DEBUG(L"core", L"Loading system dxgi.dll");

		wchar_t infoBuf[4096];
		GetSystemDirectory(infoBuf, 4096);
		lstrcatW(infoBuf, L"\\dxgi.dll");

		HMODULE sys_dxgi = LoadLibrary(infoBuf);

		DXGIFactoryCreate0 origDXGI0 = (DXGIFactoryCreate0)GetProcAddress(sys_dxgi, "CreateDXGIFactory");
		DXGIFactoryCreate1 origDXGI1 = (DXGIFactoryCreate1)GetProcAddress(sys_dxgi, "CreateDXGIFactory1");
		DXGIFactoryCreate2 origDXGI2 = (DXGIFactoryCreate2)GetProcAddress(sys_dxgi, "CreateDXGIFactory2");
		switch (ver)
		{
		case 0:
			ret = origDXGI0(riid, ppFactory);
			break;
		case 1:
			ret = origDXGI1(riid, ppFactory);
			break;
		case 2:
			ret = origDXGI2(Flags, riid, ppFactory);
			break;
		}
	}

	LOG_DEBUG(L"core", L"IDXGIFactory = %p, ret = %u", *ppFactory, ret);

	return ret;
}

HRESULT loader_core::OnDXGIFactoryCreate(UINT ver, UINT Flags, REFIID riid, void** ppFactory)
{
	innerInit();
	return RouteDXGIFactoryCreate(ver, Flags, riid, ppFactory);
}


IDirect3D9 * loader_core::OnD3DCreate(UINT sdkVer)
{
	innerInit();
	return RouteD3DCreate(sdkVer);
}

enum class WarpSelection
{
	ASK = 0,
	ON = 1,
	OFF = 2
};
WarpSelection g_warpSelection;

HRESULT loader_core::RouteD3D11CreateDeviceAndSwapChain(DX11_CREATE_FDEF)
{
	typedef HRESULT (WINAPI* D3D11CreateDeviceAndSwapChainFunc)(DX11_CREATE_FDEF);
	D3D11CreateDeviceAndSwapChainFunc d3d11_create_hook = (D3D11CreateDeviceAndSwapChainFunc)gw2al_core__query_function(GW2AL_CORE_FUNN_D3D11CREATE_HOOK);
	HRESULT ret = NULL;

#ifdef _DEBUG
	if(IsDebuggerPresent())
	{
	    Flags |= D3D11_CREATE_DEVICE_DEBUG;
	    if (g_warpSelection == WarpSelection::ASK)
		    g_warpSelection = MessageBoxA(nullptr, "Debug addon loader active. Enable WARP?", "WARP", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 | MB_TASKMODAL) == IDYES ? WarpSelection::ON : WarpSelection::OFF;

	    if (g_warpSelection == WarpSelection::ON)
	    {
		    DriverType = D3D_DRIVER_TYPE_WARP;
		    pAdapter = nullptr;
	    }
	}
#endif

	if (d3d11_create_hook)
	{
		LOG_DEBUG(L"core", L"Calling D3D11Create, hook = 0x%016llX", d3d11_create_hook);
		ret = d3d11_create_hook(DX11_CREATE_PLIST);
	}
	else
	{
		LOG_DEBUG(L"core", L"Loading system d3d11.dll");

		wchar_t infoBuf[4096];
		GetSystemDirectory(infoBuf, 4096);
		lstrcatW(infoBuf, L"\\d3d11.dll");

		HMODULE sys_d3d11 = LoadLibrary(infoBuf);

		D3D11CreateDeviceAndSwapChainFunc origDX11Create = (D3D11CreateDeviceAndSwapChainFunc)GetProcAddress(sys_d3d11, "D3D11CreateDeviceAndSwapChain");
		ret = origDX11Create(DX11_CREATE_PLIST);
	}

	LOG_DEBUG(L"core", L"ID3D11Device = 0x%016llX", *ppDevice);

	return ret;
}

HRESULT loader_core::OnD3D11CreateDeviceAndSwapChain(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext)
{
	innerInit();
	return RouteD3D11CreateDeviceAndSwapChain(DX11_CREATE_PLIST);
}

void loader_core::SignalUnload()
{
	if (SwitchState(LDR_UNLOAD))
	{
		LOG_INFO(L"core", L"Unloading core addon");

		if (gw2al_core__unload_addon(gw2al_core__hash_name((wchar_t*)L"loader_core")) != GW2AL_OK)
		{
			LOG_ERROR(L"core", L"Some addons are not unloaded!");
		}
		else {
			SwitchState(LDR_UNLOADED);

			LOG_INFO(L"core", L"Unloaded");
		}
	}
}

BOOL loader_core::SwitchState(loader_state newState)
{
	int doSwitch = 0;

	switch (state)
	{
		case LDR_DLL_LOADED:
		case LDR_UNLOADED:
			doSwitch = newState == LDR_ADDON_LOAD;
			break;
		case LDR_ADDON_LOAD:
			doSwitch = newState == LDR_INGAME;
			break;
		case LDR_INGAME:
			doSwitch = newState == LDR_UNLOAD;
			break;
		case LDR_UNLOAD:
			doSwitch = newState == LDR_UNLOADED;
			break;
		default:
			;
	}

	LOG_DEBUG(L"core", L"state switch asked: %u -> %u = %u", state, newState, doSwitch ? newState : state);

	state = doSwitch ? newState : state;

	return doSwitch;
}
