#include "stdafx.h"

FILE* logFile = NULL;

void gw2al_core__init()
{
	gw2al_core__init_func_registry();
	gw2al_core__init_addon_registry();
	gw2al_core__init_events();

	fopen_s(&logFile,"gw2al_log.txt", "w");
}

gw2al_hashed_name gw2al_core__hash_name(wchar_t * name)
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
	loader_core::instance.SignalUnload();

	if (logFile)
	{
		fflush(logFile);
		fclose(logFile);
		logFile = NULL;
	}
}

void gw2al_core__log_text(gw2al_log_level level, wchar_t* source, wchar_t* text)
{
#ifndef _DEBUG
	if (level > LL_WARN)
		return;
#endif

	if (!logFile)
		return;

	static const wchar_t* lvlPrefix[] = {
		L"[INFO] ",
		L"[ERR] ",
		L"[WARN] ",
		L"[DEBUG] "
	};

	fwrite(lvlPrefix[level], sizeof(wchar_t), wcslen(lvlPrefix[level]), logFile);	
	fwrite(source, sizeof(wchar_t), wcslen(source), logFile);
	fwrite(L" | ", sizeof(wchar_t), 3, logFile);
	fwrite(text, sizeof(wchar_t), wcslen(text), logFile);
	fwrite(L"\r\n", sizeof(wchar_t), 2, logFile);

	fflush(logFile);
}
