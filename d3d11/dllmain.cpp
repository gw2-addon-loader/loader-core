
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>

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



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    return TRUE;
}

