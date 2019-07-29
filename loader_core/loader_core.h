#pragma once
#include "stdafx.h"

typedef enum loader_state {
	LDR_DLL_LOADED,	
	LDR_ADDON_LOAD,
	LDR_INGAME,
	LDR_UNLOAD,
	LDR_UNLOADED
} loader_state;

class loader_core {

public:
	loader_core();
	~loader_core();
	
	IDirect3D9* OnD3DCreate(UINT sdkVer);
	void SignalUnload();
	
	loader_state GetCurrentState();

	void LoadAddonsFromDir(const wchar_t* dir);

private:	
	BOOL SwitchState(loader_state newState);
	loader_state state;

};

static loader_core instance;
