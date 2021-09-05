#include "stdafx.h"

//gw2addon_get_description
//gw2addon_load
//gw2addon_unload

gw2al_addon_dsc gAddonDeps[] = {
	GW2AL_CORE_DEP_ENTRY,
	{0,0,0,0,0,0}
};

gw2al_addon_dsc gAddonDsc = {
	L"custom_d3d9",
	L"sample addon to load custom d3d9.dll file",
	1,
	0,
	1,
	gAddonDeps
};

gw2al_core_vtable* gAPI;
HMODULE custom_d3d9_module;

IDirect3D9* custom_create_d3d9_func()
{
	custom_d3d9_module = LoadLibraryA("d3d9_custom.dll");

	if (!custom_d3d9_module)
		return 0;

	typedef IDirect3D9* (WINAPI* Direct3DCreate9Func)(UINT sdkver);

	Direct3DCreate9Func d3d9create_fun = (Direct3DCreate9Func)GetProcAddress(custom_d3d9_module, "Direct3DCreate9");

	if (!d3d9create_fun)
		return 0;

	IDirect3D9* res = d3d9create_fun(D3D_SDK_VERSION);

	return res;
}

gw2al_addon_dsc* gw2addon_get_description()
{
	return &gAddonDsc;
}

gw2al_api_ret gw2addon_load(gw2al_core_vtable* core_api)
{
	gAPI = core_api;

	gAPI->log_text(LL_INFO, (wchar_t*)L"custom_d3d9", (wchar_t*)L"YEET!");

	gAPI->register_function(&custom_create_d3d9_func, GW2AL_CORE_FUNN_D3DCREATE_HOOK);

	return GW2AL_OK;
}

gw2al_api_ret gw2addon_unload(int gameExiting)
{
	gAPI->unregister_function(GW2AL_CORE_FUNN_D3DCREATE_HOOK);

	if (custom_d3d9_module)
		FreeLibrary(custom_d3d9_module);

	return GW2AL_OK;
}