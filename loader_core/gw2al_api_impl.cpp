#include "stdafx.h"

FILE* logFile = NULL;
bool firstInit = true;

static const wchar_t* lvlPrefix[] = {
	L"[INFO] ",
	L"[ERR] ",
	L"[WARN] ",
	L"[DEBUG] "
};

bool gw2al_core__init()
{
	if (!firstInit)
		return false;

	gw2al_core__init_func_registry();
	gw2al_core__init_addon_registry();
	gw2al_core__init_events();

	logFile = fopen("gw2al_log.txt", "wb");

	firstInit = false;
	return true;
}

gw2al_hashed_name gw2al_core__hash_name(const wchar_t * name)
{
	unsigned int msz = lstrlenW(name)*2;

	gw2al_hashed_name hash = 0xcbf29ce484222325;
	unsigned int ctr = 0;

	while (ctr != msz)
	{
		UINT8 dataByte = ((UINT8*)name)[ctr];

		hash = hash ^ dataByte;
		hash = hash * 1099511628211;
		++ctr;
	}

	return hash;
}

void gw2al_core__client_unload()
{
	LOG_DEBUG(L"core", L"client unload recived");
	loader_core::instance.SignalUnload();

	if (logFile)
	{
		fflush(logFile);
		//megai2: no reliable way to detect client unload
		//log will be populated with new data, not created again and closed by OS on dll unload
		//so we are kinda fine to keep it opened 
		//fclose(logFile);
		//logFile = NULL;
	}
}

void gw2al_core__log_text_sync(gw2al_log_level level, const  wchar_t* source, const wchar_t* text)
{
	gw2al_core__log_text(level, source, text);

	fflush(logFile);
}


void gw2al_core__log_text(gw2al_log_level level, const  wchar_t* source, const wchar_t* text)
{
#ifndef _DEBUG
	if (level > LL_WARN)
		return;
#endif

	if (!logFile)
		return;

	fwrite(lvlPrefix[level], sizeof(wchar_t), wcslen(lvlPrefix[level]), logFile);	
	fwrite(source, sizeof(wchar_t), wcslen(source), logFile);
	fwrite(L" | ", sizeof(wchar_t), 3, logFile);
	fwrite(text, sizeof(wchar_t), wcslen(text), logFile);
	fwrite(L"\r\n", sizeof(wchar_t), 2, logFile);

	fflush(logFile);
}
