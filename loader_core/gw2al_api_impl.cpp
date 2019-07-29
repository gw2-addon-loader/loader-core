#include "stdafx.h"

void gw2al_core__init()
{
	gw2al_core__init_func_registry();
	gw2al_core__init_addon_registry();
	gw2al_core__init_events();
}

gw2al_hashed_name gw2al_core__hash_name(wchar_t * name)
{
	return gw2al_hashed_name();
}

void gw2al_core__client_unload()
{
	instance.SignalUnload();
}
