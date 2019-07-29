#include "stdafx.h"

//Direct3DCreate9
extern "C" IDirect3D9* WINAPI Direct3DCreate9(UINT SDKVersion) {	
	return instance.OnD3DCreate(SDKVersion);
}

extern "C" HRESULT WINAPI Direct3DCreate9Ex(
	_In_  UINT         SDKVersion,
	_Out_ IDirect3D9Ex **ppD3D
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