#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>

HMODULE target_lib = 0;
const wchar_t* target_lib_name = L"addonLoader.dll";

typedef IDirect3D9 * (WINAPI* fnDirect3DCreate9)(UINT SDKVersion);

void* getTargetProc(const char* procName)
{
	if (!target_lib)
	{
		target_lib = LoadLibrary(target_lib_name);
	}

	return GetProcAddress(target_lib, procName);
}


extern "C" IDirect3D9 * WINAPI Direct3DCreate9(UINT SDKVersion) {
	fnDirect3DCreate9 fun = (fnDirect3DCreate9)getTargetProc("Direct3DCreate9");
	return fun(SDKVersion);
}

extern "C" HRESULT WINAPI Direct3DCreate9Ex(
	_In_  UINT         SDKVersion,
	_Out_ IDirect3D9Ex * *ppD3D
)
{
	return 0;
}


extern "C" int WINAPI D3DPERF_BeginEvent(D3DCOLOR col, LPCWSTR wszName)
{
	return D3DERR_NOTAVAILABLE;
}

extern "C" int WINAPI D3DPERF_EndEvent(void)
{
	return D3DERR_NOTAVAILABLE;
}

extern "C" void WINAPI D3DPERF_SetMarker(D3DCOLOR col, LPCWSTR wszName)
{
}

extern "C" void WINAPI D3DPERF_SetRegion(D3DCOLOR col, LPCWSTR wszName)
{
}

extern "C" BOOL WINAPI D3DPERF_QueryRepeatFrame(void)
{
	return 0;
}

extern "C" void WINAPI D3DPERF_SetOptions(DWORD dwOptions)
{
}

extern "C" DWORD WINAPI D3DPERF_GetStatus(void)
{
	return 0;
}