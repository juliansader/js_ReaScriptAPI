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

//////////////////////////////////////////////////////////////////////////////////

static void* __vararg_JS_VKeys_GetState(void** arglist, int numparms)
{
	return (void*)(intptr_t)JS_VKeys_GetState((char*)arglist[0], (int)(intptr_t)arglist[1]);
}

static void* __vararg_JS_VKeys_GetHistory(void** arglist, int numparms)
{
	return (void*)(intptr_t)JS_VKeys_GetHistory((char*)arglist[0], (int)(intptr_t)arglist[1]);
}

static void* __vararg_JS_VKeys_ClearHistory(void** arglist, int numparms)
{
	JS_VKeys_ClearHistory();
	return nullptr;
}

static void* __vararg_JS_VKeys_Intercept(void** arglist, int numparms)
{
	return (void*)(intptr_t)JS_VKeys_Intercept((int)(intptr_t)arglist[0], (int)(intptr_t)arglist[1]);
}

////////////////////////////////////////////////////////////////////////////

