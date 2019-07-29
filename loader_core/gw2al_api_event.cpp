#include "stdafx.h"

void gw2al_core__init_events()
{
}

gw2al_api_ret gw2al_core__watch_event(gw2al_hashed_name name, gw2al_hashed_name subscriber, gw2al_api_event_handler handler, unsigned int priority)
{
	return gw2al_api_ret();
}

void gw2al_core__unwatch_event(gw2al_hashed_name name, gw2al_hashed_name subscriber)
{
}

gw2al_event_id gw2al_core__query_event(gw2al_hashed_name name)
{
	return gw2al_event_id();
}

unsigned int gw2al_core__trigger_event(gw2al_event_id id, void * data)
{
	return 0;
}
