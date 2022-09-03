#define WIN32_LEAN_AND_MEAN
#include <windows.h>

using Compat_t = void*(WINAPI *)();
Compat_t RealCompatValue = nullptr;
Compat_t RealCompatString = nullptr;

typedef HRESULT(WINAPI* DXGIFactoryCreate0)(REFIID riid, void** ppFactory);
typedef HRESULT(WINAPI* DXGIFactoryCreate1)(REFIID riid, void** ppFactory);
typedef HRESULT(WINAPI* DXGIFactoryCreate2)(UINT Flags, REFIID riid, void** ppFactory);
typedef HRESULT(WINAPI* DXGIDebugInterfaceGet1)(UINT Flags, REFIID riid, void** pDebug);

HMODULE target_lib = 0;
const wchar_t* target_lib_name = L"addonLoader.dll";

void* getTargetProc(const char* procName)
{
    if (!target_lib)
    {
        target_lib = LoadLibrary(target_lib_name);
    }

    return GetProcAddress(target_lib, procName);
}

HRESULT WINAPI CreateDXGIFactory(REFIID riid, void** ppFactory)
{
    DXGIFactoryCreate0 fun = (DXGIFactoryCreate0)getTargetProc("CreateDXGIFactory");
    return fun(riid, ppFactory);
}

HRESULT WINAPI CreateDXGIFactory1(REFIID riid, void** ppFactory)
{
    DXGIFactoryCreate1 fun = (DXGIFactoryCreate1)getTargetProc("CreateDXGIFactory1");
    return fun(riid, ppFactory);
}

HRESULT WINAPI CreateDXGIFactory2(UINT Flags, REFIID riid, void** ppFactory)
{
    DXGIFactoryCreate2 fun = (DXGIFactoryCreate2)getTargetProc("CreateDXGIFactory2");
    return fun(Flags, riid, ppFactory);
}

HRESULT DXGIGetDebugInterface1(UINT Flags, REFIID riid, void** pDebug)
{
    DXGIDebugInterfaceGet1 fun = (DXGIDebugInterfaceGet1)getTargetProc("DXGIGetDebugInterface1");
    return fun(Flags, riid, pDebug);
}

HMODULE GetDXGIModule()
{
    wchar_t infoBuf[4096];
    GetSystemDirectory(infoBuf, 4096);
    lstrcatW(infoBuf, L"\\dxgi.dll");

    return LoadLibrary(infoBuf);
}

FARPROC GetDXGIFunction(LPCSTR name)
{
    static HMODULE hmod = GetDXGIModule();
    return GetProcAddress(hmod, name);
}

#define GET_DXGI_FUNC(name) (decltype(name)*)GetDXGIFunction(#name)

extern "C" void* WINAPI CompatValue()
{
    return RealCompatValue();
}

extern "C" void* WINAPI CompatString()
{
    return RealCompatString();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  fdwReason,
                       LPVOID lpReserved
                     )
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        RealCompatValue = GET_DXGI_FUNC(CompatValue);
        RealCompatString = GET_DXGI_FUNC(CompatString);
        break;
    case DLL_PROCESS_DETACH:
        RealCompatValue = nullptr;
        RealCompatString = nullptr;
        break;
    }
}

