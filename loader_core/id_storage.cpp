#include "stdafx.h"

template<class idType, class objType>
id_storage<idType, objType>::id_storage(idType totalIdsCount)
{
	idMap.clear();
	maxId = 1;
	objArray = (objType*)malloc(sizeof(objType)*totalIdsCount);
	memset(objArray, 0, sizeof(objType)*totalIdsCount);
	idsAllocated = totalIdsCount;
}

template<class idType, class objType>
id_storage<idType, objType>::~id_storage()
{
	free(objArray);
}

template<class idType, class objType>
gw2al_api_ret id_storage<idType, objType>::register_obj(objType obj, gw2al_hashed_name name)
{
	if (idMap.find(name) != idMap.end())
		return GW2AL_IN_USE;

	unsigned short id = maxId;

	if (freeIds.empty())
	{
		if (maxId == idsAllocated)
		{
			return GW2AL_STATIC_LIMIT_HIT;
		}
		++maxId;
	}
	else {
		id = freeIds.top();
		freeIds.pop();
	}

	idMap[name] = id;
	objArray[id] = obj;

	return GW2AL_OK;
}

template<class idType, class objType>
void id_storage<idType, objType>::unregister_obj(gw2al_hashed_name name)
{
	auto idItr = idMap.find(name);

	if (idItr == idMap.end())
		return;

	unsigned short id = idItr->second;

	objArray[id] = NULL;
	freeIds.push(id);
}

template<class idType, class objType>
objType id_storage<idType, objType>::query_obj(gw2al_hashed_name name)
{
	auto idItr = idMap.find(name);

	if (idItr == idMap.end())
		return NULL;

	return objArray[idItr->second];
}

template<class idType, class objType>
objType * id_storage<idType, objType>::get_array(idType * sz)
{
	*sz = maxId;
	return objArray;
}

template class id_storage<unsigned short, void*>;
template class id_storage<unsigned short, gw2al_addon*>;