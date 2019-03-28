#pragma once

extern "C" REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t *rec);

void  JS_ReaScriptAPI_Version(double* versionOut);

void  JS_Localize(const char* USEnglish, const char* LangPackSection, char* translationOut, int translationOut_sz);
