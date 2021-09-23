#include "stdafx.h"

HRESULT WINAPI CreateDXGIFactory(REFIID riid, void** ppFactory)
{
	return loader_core::instance.OnDXGIFactoryCreate(0, 0, riid, ppFactory);
}

HRESULT WINAPI CreateDXGIFactory1(REFIID riid, void** ppFactory)
{
	return loader_core::instance.OnDXGIFactoryCreate(1, 0, riid, ppFactory);
}

HRESULT WINAPI CreateDXGIFactory2(UINT Flags, REFIID riid, void** ppFactory)
{
	return loader_core::instance.OnDXGIFactoryCreate(2, Flags, riid, ppFactory);
}
