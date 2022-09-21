
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>

using D3D11CoreCreateDevice_t = HRESULT (WINAPI *)(
    IDXGIFactory * pFactory,
    IDXGIAdapter * pAdapter,
    UINT Flags,
    const D3D_FEATURE_LEVEL * pFeatureLevels,
    UINT FeatureLevels,
    ID3D11Device * *ppDevice);
D3D11CoreCreateDevice_t RealD3D11CoreCreateDevice = nullptr;

using D3D11CoreCreateLayeredDevice_t = HRESULT (WINAPI *)(
    const void* unknown0,
    DWORD unknown1,
    const void* unknown2,
    REFIID riid,
    void** ppvObj);
D3D11CoreCreateLayeredDevice_t RealD3D11CoreCreateLayeredDevice = nullptr;

using D3D11CoreGetLayeredDeviceSize_t = SIZE_T (WINAPI *)(const void* unknown0, DWORD unknown1);
D3D11CoreGetLayeredDeviceSize_t RealD3D11CoreGetLayeredDeviceSize = nullptr;

using D3D11CoreRegisterLayers_t = HRESULT (WINAPI *)(const void* unknown0, DWORD unknown1);
D3D11CoreRegisterLayers_t RealD3D11CoreRegisterLayers = nullptr;

#define DX11_CREATE_PLIST pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext

typedef HRESULT(WINAPI* D3D11CreateDeviceAndSwapChainFunc)(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext);

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


extern "C" HRESULT WINAPI D3D11CreateDevice(IDXGIAdapter * pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL * pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, ID3D11Device * *ppDevice, D3D_FEATURE_LEVEL * pFeatureLevel, ID3D11DeviceContext * *ppImmediateContext)
{
    return D3D11CreateDeviceAndSwapChain(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, nullptr, nullptr, ppDevice, pFeatureLevel, ppImmediateContext);
}

extern "C" HRESULT WINAPI D3D11CreateDeviceAndSwapChain(IDXGIAdapter * pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL * pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, const DXGI_SWAP_CHAIN_DESC * pSwapChainDesc, IDXGISwapChain * *ppSwapChain, ID3D11Device * *ppDevice, D3D_FEATURE_LEVEL * pFeatureLevel, ID3D11DeviceContext * *ppImmediateContext)
{
    D3D11CreateDeviceAndSwapChainFunc fun = (D3D11CreateDeviceAndSwapChainFunc)getTargetProc("D3D11CreateDeviceAndSwapChain");

    return fun(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
}

HMODULE GetD3D11Module()
{
    wchar_t infoBuf[4096];
    GetSystemDirectory(infoBuf, 4096);
    lstrcatW(infoBuf, L"\\d3d11.dll");

    auto existingModule = GetModuleHandle(infoBuf);
    if(existingModule)
        return existingModule;

    return LoadLibrary(infoBuf);
}

void FreeD3D11Module() {
    wchar_t infoBuf[4096];
    GetSystemDirectory(infoBuf, 4096);
    lstrcatW(infoBuf, L"\\d3d11.dll");

    FreeLibrary(GetModuleHandle(infoBuf));
}

FARPROC GetD3D11Function(LPCSTR name)
{
    static HMODULE hmod = GetD3D11Module();
    return GetProcAddress(hmod, name);
}

#define GET_D3D11_FUNC(name) (decltype(name)*)GetD3D11Function(#name)

bool ShouldTryLoading = true;
void LoadAllFunctions();

extern "C" HRESULT WINAPI D3D11CoreCreateDevice(
    IDXGIFactory * pFactory,
    IDXGIAdapter * pAdapter,
    UINT Flags,
    const D3D_FEATURE_LEVEL * pFeatureLevels,
    UINT FeatureLevels,
    ID3D11Device * *ppDevice)
{
    if(ShouldTryLoading)
        LoadAllFunctions();
    return RealD3D11CoreCreateDevice(pFactory, pAdapter, Flags, pFeatureLevels, FeatureLevels, ppDevice);
}

extern "C" HRESULT WINAPI D3D11CoreCreateLayeredDevice(
    const void* unknown0,
    DWORD unknown1,
    const void* unknown2,
    REFIID riid,
    void** ppvObj)
{
    if(ShouldTryLoading)
        LoadAllFunctions();
    return RealD3D11CoreCreateLayeredDevice(unknown0, unknown1, unknown2, riid, ppvObj);
}

extern "C" SIZE_T WINAPI D3D11CoreGetLayeredDeviceSize(const void* unknown0, DWORD unknown1)
{
    if(ShouldTryLoading)
        LoadAllFunctions();
    return RealD3D11CoreGetLayeredDeviceSize(unknown0, unknown1);
}

extern "C" HRESULT WINAPI D3D11CoreRegisterLayers(const void* unknown0, DWORD unknown1)
{
    if(ShouldTryLoading)
        LoadAllFunctions();
    return RealD3D11CoreRegisterLayers(unknown0, unknown1);
}

void LoadAllFunctions() {
    RealD3D11CoreCreateDevice = GET_D3D11_FUNC(D3D11CoreCreateDevice);
    RealD3D11CoreCreateLayeredDevice = GET_D3D11_FUNC(D3D11CoreCreateLayeredDevice);
    RealD3D11CoreGetLayeredDeviceSize = GET_D3D11_FUNC(D3D11CoreGetLayeredDeviceSize);
    RealD3D11CoreRegisterLayers = GET_D3D11_FUNC(D3D11CoreRegisterLayers);

    ShouldTryLoading = false;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  fdwReason,
                       LPVOID lpReserved
                     )
{
    if(fdwReason == DLL_PROCESS_DETACH)
    {
        ShouldTryLoading = true;
        FreeD3D11Module();
    }

    return true;
}
