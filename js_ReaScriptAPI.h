#pragma once

extern "C" REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t *rec);

void  JS_ReaScriptAPI_Version(double* versionOut);

void  JS_Localize(const char* USEnglish, const char* LangPackSection, char* translationOut, int translationOut_sz);

int   JS_VKeys_Callback(MSG* event, accelerator_register_t*);
bool  JS_VKeys_GetState(char* stateOutNeedBig, int stateOutNeedBig_sz);
void  JS_VKeys_ClearHistory();
bool  JS_VKeys_GetHistory(char* stateOutNeedBig, int stateOutNeedBig_sz);
int   JS_VKeys_Intercept(int keyCode, int intercept);
