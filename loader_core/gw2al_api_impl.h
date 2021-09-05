#pragma once
#include "stdafx.h"

typedef struct gw2al_addon {
	gw2al_addon_dsc* desc;
	HMODULE addonLib;
	gw2al_addon_get_dsc_proc getDesc;
	gw2al_addon_load_proc load;
	gw2al_addon_unload_proc unload;
} gw2al_addon;

typedef struct gw2al_event_subscriber {
	gw2al_hashed_name subscriber;
	unsigned int priority;
	gw2al_api_event_handler handler;	
} gw2al_event_subscriber;

typedef struct gw2al_event_dsc {
	gw2al_hashed_name name;
	gw2al_event_id id;
	unsigned int subCount;
	unsigned int activeSubCount;
	gw2al_event_subscriber* subs;
} gw2al_event_dsc;

bool gw2al_core__init();
void gw2al_core__init_func_registry();
void gw2al_core__init_addon_registry();
void gw2al_core__init_events();

//converts string to hash for usage in other functions
gw2al_hashed_name gw2al_core__hash_name(const wchar_t* name);

//register/unregister user functions to be called by other addons
gw2al_api_ret gw2al_core__register_function(void* function, gw2al_hashed_name name);
void gw2al_core__unregister_function(gw2al_hashed_name name);

//query function pointer from registered list
void* gw2al_core__query_function(gw2al_hashed_name name);

//fills table of functions using query_function
void gw2al_core__fill_vtable(const gw2al_hashed_name* nameList, void** vtable);

//functions to unload/load addons 
gw2al_api_ret gw2al_core__unload_addon(gw2al_hashed_name name);
gw2al_api_ret gw2al_core__load_addon(const wchar_t* name);

//function to get currently loaded addon description
gw2al_addon_dsc* gw2al_core__query_addon(gw2al_hashed_name name);

//simple event api 
//watch event can add a number of handlers on event name with priority 
//query event will get internal event id to speedup trigger_event calls

gw2al_event_id gw2al_core__query_event(gw2al_hashed_name name);
gw2al_api_ret gw2al_core__watch_event(gw2al_event_id id, gw2al_hashed_name subscriber, gw2al_api_event_handler handler, unsigned int priority);
void gw2al_core__unwatch_event(gw2al_event_id id, gw2al_hashed_name subscriber);
unsigned int gw2al_core__trigger_event(gw2al_event_id id, void* data);

void gw2al_core__client_unload();

void gw2al_core__log_text_sync(gw2al_log_level level, const wchar_t* source, const wchar_t* text);
void gw2al_core__log_text(gw2al_log_level level, const wchar_t* source, const wchar_t* text);