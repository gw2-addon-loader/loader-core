#include "stdafx.h"
#include "build_version.h"

id_storage<unsigned short, gw2al_addon*> addonStorage(0xFFFF);

gw2al_addon coreAddon;
gw2al_addon_dsc coreAddonDsc;
gw2al_hashed_name coreAddonName;

gw2al_core_vtable api_vtable;

gw2al_api_ret gw2al_core__inner_addon_unload(int gameExiting)
{
	return GW2AL_OK;
}

void gw2al_core__init_addon_registry()
{
	gw2al_hashed_name api_nameList[] = {
		GW2AL_CORE_FUNN_HASH_NAME,
		GW2AL_CORE_FUNN_REG_FUN,
		GW2AL_CORE_FUNN_UNREG_FUN,
		GW2AL_CORE_FUNN_QUERY_FUN,
		GW2AL_CORE_FUNN_FILL_VTBL,
		GW2AL_CORE_FUNN_UNLOAD_ADDON,
		GW2AL_CORE_FUNN_LOAD_ADDON,
		GW2AL_CORE_FUNN_QUERY_ADDON,
		GW2AL_CORE_FUNN_WATCH_EVENT,
		GW2AL_CORE_FUNN_UNWATCH_EVENT,
		GW2AL_CORE_FUNN_QUERY_EVENT,
		GW2AL_CORE_FUNN_TRIGGER_EVENT,
		GW2AL_CORE_FUNN_CLIENT_UNLOAD,
		GW2AL_CORE_FUNN_LOG_TEXT_SYNC,
		GW2AL_CORE_FUNN_LOG_TEXT,
		0
	};

	gw2al_core__fill_vtable(api_nameList, (void**)&api_vtable);

	coreAddonDsc.dependList = 0;
	coreAddonDsc.description = L"core addon loading library";
	coreAddonDsc.name = L"loader_core";
	coreAddonDsc.majorVer = LOADER_CORE_VER_MAJOR;
	coreAddonDsc.minorVer = LOADER_CORE_VER_MINOR;
	coreAddonDsc.revision = LOADER_CORE_VER_REV;

	coreAddon.addonLib = 0;
	coreAddon.desc = &coreAddonDsc;
	coreAddon.unload = &gw2al_core__inner_addon_unload;
	coreAddonName = gw2al_core__hash_name((wchar_t*)coreAddonDsc.name);

	addonStorage.register_obj(&coreAddon, coreAddonName);
}

gw2al_api_ret gw2al_core__unload_addon(gw2al_hashed_name name)
{
	gw2al_addon* addon = addonStorage.query_obj(name);

	if (!addon)
		return GW2AL_NOT_FOUND;

	LOG_DEBUG(L"core", L"Unloading addon %s", addon->desc->name);

	unsigned short i;
	bool pendingDeps = false;

	gw2al_addon** list = addonStorage.get_array(&i);

	while (i)
	{
		if (!list[i])
		{
			--i;
			continue;
		}

		gw2al_addon_dsc* depList = list[i]->desc->dependList;

		if (depList)
		{
			unsigned int j = 0;
			while (depList[j].name)
			{
				if (name == gw2al_core__hash_name((wchar_t*)depList[j].name))
				{
					if (gw2al_core__unload_addon(gw2al_core__hash_name((wchar_t*)list[i]->desc->name)) != GW2AL_OK)
						pendingDeps = true;
					break;
				}
				++j;
			}
			
		}

		--i;
	}

	if (pendingDeps)
		return GW2AL_DEP_STILL_LOADED;

	gw2al_api_ret ret = addon->unload(loader_core::instance.GetCurrentState() != LDR_INGAME);

	if (ret == GW2AL_OK)
	{
		//special case for core addon as we can't unload it here
		if (name == coreAddonName)
			return ret;

		addonStorage.unregister_obj(name);

		FreeLibrary(addon->addonLib);

		free(addon);
	}
	else {
		LOG_ERROR(L"core", L"Unloading addon %s failed with error %u", addon->desc->name, ret);
	}
	
	return ret;
}

gw2al_api_ret gw2al_core__load_addon(const wchar_t * name)
{
	LOG_DEBUG(L"core", L"Loading addon %s", name);

	gw2al_hashed_name nameH = gw2al_core__hash_name(name);

	if (gw2al_core__query_addon(nameH))
		return GW2AL_OK;

	static wchar_t dll_name[4096];
	swprintf(dll_name, 4096, L"addons/%s/gw2addon_%s.dll", name, name);
	
	gw2al_addon laddon;

	laddon.addonLib = LoadLibrary(dll_name);

	if (laddon.addonLib == NULL)
		return GW2AL_NOT_FOUND;

	laddon.getDesc = (gw2al_addon_get_dsc_proc)GetProcAddress(laddon.addonLib, "gw2addon_get_description");
	laddon.load = (gw2al_addon_load_proc)GetProcAddress(laddon.addonLib, "gw2addon_load");
	laddon.unload = (gw2al_addon_unload_proc)GetProcAddress(laddon.addonLib, "gw2addon_unload");

	if (!laddon.getDesc || !laddon.load || !laddon.unload)
	{
		FreeLibrary(laddon.addonLib);
		return GW2AL_BAD_DLL;
	}

	laddon.desc = laddon.getDesc();

	if (laddon.desc->dependList)
	{
		unsigned int i = 0;
		while (laddon.desc->dependList[i].name)
		{
			gw2al_addon_dsc* depDsc = gw2al_core__query_addon(gw2al_core__hash_name((wchar_t*)laddon.desc->dependList[i].name));

			if (!depDsc)
			{
				LOG_INFO(L"core", L"Loading dependency \"%s\" for addon \"%s\"", laddon.desc->dependList[i].name, name);
				if (gw2al_core__load_addon((wchar_t*)laddon.desc->dependList[i].name) != GW2AL_OK)
				{
					LOG_WARNING(L"core", L"Addon \"%s\" is not loaded due to missing dependency \"%s\"", name, laddon.desc->dependList[i].name);
					FreeLibrary(laddon.addonLib);
					return GW2AL_DEP_NOT_LOADED;
				}

				depDsc = gw2al_core__query_addon(gw2al_core__hash_name((wchar_t*)laddon.desc->dependList[i].name));
			}

			if ((depDsc->majorVer != laddon.desc->dependList[i].majorVer) || (depDsc->minorVer < laddon.desc->dependList[i].minorVer))
			{
				LOG_WARNING(L"core", L"Addon \"%s\" is not loaded due to wrong dependency version \"%s\". Require v%u.%u(r%u) but have v%u.%u(r%u)", name, laddon.desc->dependList[i].name,
					laddon.desc->dependList[i].majorVer, laddon.desc->dependList[i].minorVer, laddon.desc->dependList[i].revision,
					depDsc->majorVer, depDsc->minorVer, depDsc->revision
					);
				FreeLibrary(laddon.addonLib);
				return GW2AL_DEP_OUTDATED;
			}

			++i;
		}
	}

	gw2al_api_ret ret = laddon.load(&api_vtable);

	if (ret != GW2AL_OK)
	{
		FreeLibrary(laddon.addonLib);
		return ret;
	}

	gw2al_addon* addon = (gw2al_addon*)malloc(sizeof(gw2al_addon));

	*addon = laddon;

	addonStorage.register_obj(addon, nameH);

	LOG_INFO(L"core", L"Loaded %s v%u.%u r%u", laddon.desc->name, laddon.desc->majorVer, laddon.desc->minorVer, laddon.desc->revision);
	
	return ret;
}

gw2al_addon_dsc * gw2al_core__query_addon(gw2al_hashed_name name)
{
	gw2al_addon* addon = addonStorage.query_obj(name);

	if (addon)
		return addon->desc;
	else
		return NULL;
}
