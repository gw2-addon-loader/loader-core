#include "stdafx.h"

id_storage<unsigned short, void*> funcStorage(0xFFFF);

void gw2al_core__init_func_registry()
{
	gw2al_core__register_function(gw2al_core__hash_name, GW2AL_CORE_FUNN_HASH_NAME);
	gw2al_core__register_function(gw2al_core__register_function, GW2AL_CORE_FUNN_REG_FUN);
	gw2al_core__register_function(gw2al_core__unregister_function, GW2AL_CORE_FUNN_UNREG_FUN);
	gw2al_core__register_function(gw2al_core__query_function, GW2AL_CORE_FUNN_QUERY_FUN);
	gw2al_core__register_function(gw2al_core__fill_vtable, GW2AL_CORE_FUNN_FILL_VTBL);
	gw2al_core__register_function(gw2al_core__unload_addon, GW2AL_CORE_FUNN_UNLOAD_ADDON);
	gw2al_core__register_function(gw2al_core__load_addon, GW2AL_CORE_FUNN_LOAD_ADDON);
	gw2al_core__register_function(gw2al_core__query_addon, GW2AL_CORE_FUNN_QUERY_ADDON);
	gw2al_core__register_function(gw2al_core__watch_event, GW2AL_CORE_FUNN_WATCH_EVENT);
	gw2al_core__register_function(gw2al_core__unwatch_event, GW2AL_CORE_FUNN_UNWATCH_EVENT);
	gw2al_core__register_function(gw2al_core__query_event, GW2AL_CORE_FUNN_QUERY_EVENT);
	gw2al_core__register_function(gw2al_core__trigger_event, GW2AL_CORE_FUNN_TRIGGER_EVENT);
	gw2al_core__register_function(gw2al_core__client_unload, GW2AL_CORE_FUNN_CLIENT_UNLOAD);	 
	gw2al_core__register_function(gw2al_core__log_text, GW2AL_CORE_FUNN_LOG_TEXT);
}

gw2al_api_ret gw2al_core__register_function(void * function, gw2al_hashed_name name)
{	
	LOG_DEBUG(L"core", L"registered function %016llX = %016llX", name, function);
	return funcStorage.register_obj(function, name);
}

void gw2al_core__unregister_function(gw2al_hashed_name name)
{
	funcStorage.unregister_obj(name);
}

void * gw2al_core__query_function(gw2al_hashed_name name)
{
	return funcStorage.query_obj(name);
}

void gw2al_core__fill_vtable(const gw2al_hashed_name * nameList, void ** vtable)
{
	unsigned int i = 0;
	while (nameList[i] != 0)
	{
		vtable[i] = gw2al_core__query_function(nameList[i]);
		++i;
	}
}

