#pragma once
#include "stdafx.h"
#include <unordered_map>
#include <stack> 

template <class idType, class objType>
class id_storage
{
public:
	id_storage(idType totalIdsCount);
	~id_storage();

	gw2al_api_ret register_obj(objType obj, gw2al_hashed_name name);
	void unregister_obj(gw2al_hashed_name name);

	objType query_obj(gw2al_hashed_name name);

	objType* get_array(idType* sz);

private:
	std::unordered_map<gw2al_hashed_name, idType> idMap;
	std::stack<idType> freeIds;
	objType* objArray;
	idType maxId;
	idType idsAllocated;
};

