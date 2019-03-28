#pragma once

extern "C" REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t *rec);

void  JS_ReaScriptAPI_Version(double* versionOut);

void  JS_Localize(const char* USEnglish, const char* LangPackSection, char* translationOut, int translationOut_sz);

int   JS_VKeys_Callback(MSG* event, accelerator_register_t*);
bool  JS_VKeys_GetState(char* stateOutNeedBig, int stateOutNeedBig_sz);
void  JS_VKeys_ClearHistory();
bool  JS_VKeys_GetHistory(char* stateOutNeedBig, int stateOutNeedBig_sz);
int   JS_VKeys_Intercept(int keyCode, int intercept);

int   JS_WindowMessage_Intercept(void* windowHWND, const char* message, bool passThrough);
int   JS_WindowMessage_InterceptList(void* windowHWND, const char* messages);
int   JS_WindowMessage_PassThrough(void* windowHWND, const char* message, bool passThrough);
bool  JS_WindowMessage_ListIntercepts(void* windowHWND, char* listOutNeedBig, int listOutNeedBig_sz);
bool  JS_WindowMessage_Post(void* windowHWND, const char* message, int wParamLow, int wParamHigh, int lParamLow, int lParamHigh);
int   JS_WindowMessage_Send(void* windowHWND, const char* message, int wParamLow, int wParamHigh, int lParamLow, int lParamHigh);
bool  JS_WindowMessage_Peek(void* windowHWND, const char* message, bool* passedThroughOut, double* timeOut, int* wParamLowOut, int* wParamHighOut, int* lParamLowOut, int* lParamHighOut);
int   JS_WindowMessage_Release(void* windowHWND, const char* messages);
void  JS_WindowMessage_ReleaseWindow(void* windowHWND);
void  JS_WindowMessage_ReleaseAll();
static void JS_WindowMessage_RestoreOrigProc(HWND hwnd);
static int  JS_WindowMessage_CreateNewMap(HWND hwnd);
bool  JS_Window_OnCommand(void* windowHWND, int commandID);

void* JS_LICE_CreateBitmap(bool isSysBitmap, int width, int height);
int   JS_LICE_GetHeight(void* bitmap);
int   JS_LICE_GetWidth(void* bitmap);
void* JS_LICE_GetDC(void* bitmap);
void  JS_LICE_DestroyBitmap(LICE_IBitmap* bitmap);
