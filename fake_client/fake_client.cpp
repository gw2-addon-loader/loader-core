#include "pch.h"
#include <iostream>

int main()
{
	std::cout << "fake_client start!\n"; 

	std::cout << "Loading d3d9.dll \n";
	
	HMODULE d3d9md = LoadLibrary(L"d3d9.dll");

	if (!d3d9md)
	{
		std::cout << "d3d9.dll can't be loaded for some reason.\n";
		return -1;
	}

	typedef IDirect3D9* (WINAPI* Direct3DCreate9Func)(UINT sdkver);

	Direct3DCreate9Func origDirect3DCreate9 = (Direct3DCreate9Func)GetProcAddress(d3d9md, "Direct3DCreate9");

	if (!origDirect3DCreate9)
	{
		std::cout << "d3d9.dll Direct3DCreate9 export not found.\n";
		return -1;
	}

	std::cout << "calling Direct3DCreate9 \n";

	IDirect3D9* res = origDirect3DCreate9(D3D_SDK_VERSION);

	std::cout << "called Direct3DCreate9 \n";

	std::cout << "exiting \n";

	res->Release();

	std::cout << "d3d9 com released \n";

	FreeLibrary(d3d9md);

	std::cout << "d3d9 lib released \n";

	std::cout << "All fine! \n";

	system("pause");

	return 0;
}