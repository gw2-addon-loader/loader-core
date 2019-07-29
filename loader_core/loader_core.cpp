#include "stdafx.h"

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
				gw2al_core__load_addon(fdFile.cFileName);
			}
		}
	} while (FindNextFile(hFind, &fdFile));

	FindClose(hFind);
}

IDirect3D9 * loader_core::OnD3DCreate(UINT sdkVer)
{
	if (!SwitchState(LDR_ADDON_LOAD))
		return nullptr;

	gw2al_core__init();

	LoadAddonsFromDir(L"..\\addons");

	IDirect3D9* (*d3d9_create_hook)() = (IDirect3D9* (*)())gw2al_core__query_function(GW2AL_CORE_FUNN_D3DCREATE_HOOK);

	if (d3d9_create_hook)
		return d3d9_create_hook();
	else {
		wchar_t infoBuf[4096];
		GetSystemDirectory(infoBuf, 4096);
		lstrcatW(infoBuf, L"\\d3d9.dll");

		HMODULE sys_d3d9 = LoadLibrary(infoBuf);

		typedef IDirect3D9* (WINAPI* Direct3DCreate9Func)(UINT sdkver);

		Direct3DCreate9Func origDirect3DCreate9 = (Direct3DCreate9Func)GetProcAddress(sys_d3d9, "Direct3DCreate9");
		IDirect3D9* res = origDirect3DCreate9(sdkVer);

		return res;
	}	

	SwitchState(LDR_INGAME);
}

void loader_core::SignalUnload()
{
	if (SwitchState(LDR_UNLOAD))
	{
		gw2al_core__unload_addon(gw2al_core__hash_name((wchar_t*)L"loader_core"));
		SwitchState(LDR_UNLOADED);
	}
}

BOOL loader_core::SwitchState(loader_state newState)
{
	switch (state)
	{
		case LDR_DLL_LOADED:
			return newState == LDR_ADDON_LOAD;			
		case LDR_ADDON_LOAD:
			return newState == LDR_INGAME;
		case LDR_INGAME:
			return newState == LDR_UNLOAD;
		case LDR_UNLOAD:
			return newState == LDR_UNLOADED;
		default:
			return 0;
	}
	return 0;
}
