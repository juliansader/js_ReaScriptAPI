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

static void* __vararg_JS_WindowMessage_Post(void** arglist, int numparms)
{
  return (void*)(intptr_t)JS_WindowMessage_Post((void*)arglist[0], (const char*)arglist[1], (int)(intptr_t)arglist[2], (int)(intptr_t)arglist[3], (int)(intptr_t)arglist[4], (int)(intptr_t)arglist[5]);
}

static void* __vararg_JS_Window_OnCommand(void** arglist, int numparms)
{
	return (void*)JS_Window_OnCommand((void*)arglist[0], (int)(intptr_t)arglist[1]);
}

static void* __vararg_JS_WindowMessage_Send(void** arglist, int numparms)
{
  return (void*)(intptr_t)JS_WindowMessage_Send((void*)arglist[0], (const char*)arglist[1], (int)(intptr_t)arglist[2], (int)(intptr_t)arglist[3], (int)(intptr_t)arglist[4], (int)(intptr_t)arglist[5]);
}

static void* __vararg_JS_WindowMessage_Peek(void** arglist, int numparms)
{
  return (void*)(intptr_t)JS_WindowMessage_Peek((void*)arglist[0], (const char*)arglist[1], (bool*)arglist[2], (double*)arglist[3], (int*)arglist[4], (int*)arglist[5], (int*)arglist[6], (int*)arglist[7]);
}

static void* __vararg_JS_WindowMessage_Intercept(void** arglist, int numparms)
{
  return (void*)(intptr_t)JS_WindowMessage_Intercept((void*)arglist[0], (const char*)arglist[1], (bool)arglist[2]);
}

static void* __vararg_JS_WindowMessage_InterceptList(void** arglist, int numparms)
{
	return (void*)(intptr_t)JS_WindowMessage_InterceptList((void*)arglist[0], (const char*)arglist[1]);
}

static void* __vararg_JS_WindowMessage_PassThrough(void** arglist, int numparms)
{
	return (void*)(intptr_t)JS_WindowMessage_PassThrough(arglist[0], (const char*)arglist[1], (bool)arglist[2]);
}

static void* __vararg_JS_WindowMessage_ListIntercepts(void** arglist, int numparms)
{
  return (void*)(intptr_t)JS_WindowMessage_ListIntercepts((void*)arglist[0], (char*)arglist[1], (int)(intptr_t)arglist[2]);
}

static void* __vararg_JS_WindowMessage_Release(void** arglist, int numparms)
{
  return (void*)(intptr_t)JS_WindowMessage_Release((void*)arglist[0], (const char*)arglist[1]);
}

static void* __vararg_JS_WindowMessage_ReleaseWindow(void** arglist, int numparms)
{
  JS_WindowMessage_ReleaseWindow((void*)arglist[0]);
  return NULL;
}

static void* __vararg_JS_WindowMessage_ReleaseAll(void** arglist, int numparms)
{
  JS_WindowMessage_ReleaseAll();
  return NULL;
}
