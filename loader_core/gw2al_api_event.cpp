#include "stdafx.h"

id_storage<gw2al_event_id, gw2al_event_dsc*> eventStorage(0xFFFF);

void gw2al_core__init_events()
{
}

gw2al_api_ret gw2al_core__watch_event(gw2al_event_id id, gw2al_hashed_name subscriber, gw2al_api_event_handler handler, unsigned int priority)
{
	gw2al_event_dsc* evDsc = eventStorage.get_obj(id);

	LOG_DEBUG(L"core", L"watch at event %lu by subscriber 0x%016llX with handler 0x%016llX and priority %u", id, subscriber, handler, priority);

	unsigned int subc = evDsc->subCount;
	unsigned int subId = subc;

	for (int i = 0; i != subc; ++i)
	{
		if (evDsc->subs[i].handler == NULL)
		{
			subId = i;
			break;
		}
	}

	if (subId == subc)
	{
		++subc;
		evDsc->subCount = subc;
		UINT subArrSz = subc * sizeof(gw2al_event_subscriber);
		evDsc->subs = (gw2al_event_subscriber*)realloc(evDsc->subs, subArrSz);		
	}

	gw2al_event_subscriber* evSub = &evDsc->subs[subId];

	evSub->handler = handler;
	evSub->priority = priority;
	evSub->subscriber = subscriber;

	//sort priority

	for (int i = 0; i != subc; ++i)
	{	
		for (int j = i; j != subc; ++j)
		{
			if (evDsc->subs[i].priority < evDsc->subs[j].priority)
			{
				gw2al_event_subscriber swp = evDsc->subs[i];
				evDsc->subs[i] = evDsc->subs[j];
				evDsc->subs[j] = swp;
			}
		}
	}
	
	return GW2AL_OK;
}

void gw2al_core__unwatch_event(gw2al_event_id id, gw2al_hashed_name subscriber)
{
	LOG_DEBUG(L"core", L"remove event/watch id %lu sub 0x%016llX", id, subscriber);

	gw2al_event_dsc* evDsc = eventStorage.get_obj(id);

	if (subscriber == 0)
	{
		eventStorage.unregister_obj(evDsc->name);

		free(evDsc->subs);
		free(evDsc);
	}

	unsigned int subc = evDsc->subCount;

	for (int i = 0; i != subc; ++i)
	{
		if (evDsc->subs[i].subscriber == subscriber)
		{
			evDsc->subs[i].handler = NULL;
			evDsc->subs[i].priority = 0;
		}
	}
}

gw2al_event_id gw2al_core__query_event(gw2al_hashed_name name)
{
	gw2al_event_dsc* evDsc = eventStorage.query_obj(name);

	if (!evDsc)
	{		
		evDsc = (gw2al_event_dsc*)malloc(sizeof(gw2al_event_dsc));
		evDsc->name = name;

		//megai2: preallocate a bit
		evDsc->subCount = 10;
		UINT subArrSz = evDsc->subCount * sizeof(gw2al_event_subscriber);
		evDsc->subs = (gw2al_event_subscriber*)malloc(subArrSz);
		memset(evDsc->subs, 0, subArrSz);

		evDsc->id = eventStorage.register_new_obj(evDsc, name);

		LOG_DEBUG(L"core", L"new event %016llX = %lu", name, evDsc->id);
	}

	return evDsc->id;
}

unsigned int gw2al_core__trigger_event(gw2al_event_id id, void * data)
{
	gw2al_event_dsc* evDsc = eventStorage.get_obj(id);

	unsigned int subc = evDsc->subCount;

	for (int i = 0; i != subc; ++i)
	{
		evDsc->subs[i].handler(data);
	}

	return subc;
}
