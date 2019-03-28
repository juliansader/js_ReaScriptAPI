#pragma once

#ifdef _WIN32
#pragma warning(disable:4800) // disable "forcing value to bool..." warnings
#endif


static void* __vararg_JS_ReaScriptAPI_Version(void** arglist, int numparms)
{
	JS_ReaScriptAPI_Version((double*)arglist[0]);
	return NULL;
}

static void* __vararg_JS_Localize(void** arglist, int numparms)
{
	JS_Localize((const char*)arglist[0], (const char*)arglist[1], (char*)arglist[2], (int)(intptr_t)arglist[3]);
	return NULL;
}

