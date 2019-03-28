#include "stdafx.h"

using namespace std;

// This function is called when REAPER loads or unloads the extension.
// If rec != nil, the extenstion is being loaded.  If rec == nil, the extension is being UNloaded.
extern "C" REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t *rec)
{
	if (rec)
	{
		// First, import REAPER's C++ API functions for use in this extension.
		//		Load all listed API functions at once.
		if (REAPERAPI_LoadAPI(rec->GetFunc) != 0)
		{
			// This is the WIN32 / swell MessageBox, not REAPER's API MB.  This should create a separate window that is listed in the taskbar,
			//		and more easily visible behind REAPER's splash screen.
			MessageBox(NULL, "Unable to import default API functions.\n\nNOTE:\nThis extension requires REAPER v5.965 or later.", "ERROR: js_ReaScriptAPI extension", 0);  //fprintf(stderr, "Unable to import API functions.\n");
			return 0;
		}
		

		// Don't know what this does, but apparently it's necessary for the localization functions.
		IMPORT_LOCALIZE_RPLUG(rec);

		// Functions imported, continue initing plugin by exporting this extension's functions to ReaScript API.

		// Each function's defstring will temporarily be contructed in temp[]
		char temp[10000];
		int i, countZeroes;

		for (APIdef& f : aAPIdefs)
		{
			// REAPER uses a \0-separated string. sprintf cannot print \0, so must temporarily print \r and replace later.
			snprintf(temp, sizeof(temp), "%s\n%s\n%s\n%s", f.ret_val, f.parm_types, f.parm_names, f.help);
			temp[sizeof(temp) - 1] = '\0';
			// Create permanent copy of temp string, so that REAPER can access it later again.
			f.defstring = strdup(temp);
			// Replace the three \r with \0.
			i = 0; countZeroes = 0; while (countZeroes < 3) { if (f.defstring[i] == '\n') { f.defstring[i] = 0; countZeroes++; } i++; }
			// Each function must be registered in three ways:
			// APIdef_... provides for converting parameters to vararg format, and for help text in API
			plugin_register(f.regkey_def, (void*)f.defstring);
			// API_... for exposing to other extensions, and for IDE to recognize and color functions while typing 
			plugin_register(f.regkey_func, f.func);
			// APIvarag_... for exporting to ReaScript API
			plugin_register(f.regkey_vararg, f.func_vararg);
		}

		// Construct mapMsgToWM_ as inverse of mapWM_ToMsg
		for (auto& i : Julian::mapWM_toMsg)
			Julian::mapMsgToWM_.emplace(i.second, i.first);

		//plugin_register("accelerator", &(Julian::sAccelerator));

		return 1; // success
	}
	// Does an extension need to do anything when unloading?  
	// To prevent memory leaks, perhaps try to delete any stuff that may remain in memory?
	// On Windows, LICE bitmaps are automatically destroyed when REAPER quits, but to make extra sure, this function will destroy them explicitly.
	// Why store stuff in extra sets?  For some unexplained reason REAPER crashes if I try to destroy LICE bitmaps explicitly. And for another unexplained reason, this roundabout way works...
	else 
	{

		return 0;
	}
}

/*
v0.963:
 * Fix bug in Find functions in macOS.
 * New optional bool parameter for Mouse_LoadCursorFromFile. If not true, function will try to re-use previously loaded cursor.
 * Window_FindEx function.
v0.964:
 * Implement IsWindow for Linux and macOS.
v0.970
 * Linux and macOS: Improved IsWindow.
 * Windows OS: File dialogs accept forward slash.
 * Windows OS: BrowseForOpenFiles returns folder with terminal slash.
 * New functions: Memory allocation and access.
 * WindowMessage functions: Recognize ComboBox message names such as "CB_GETCURSEL".
v0.971
 * Fix possible memory leak in File dialogs.
v0.972
 * macOS: Fixed GetClientRect.
 * WindowsOS: Confirmation dialogs for BrowseForSaveFile and BrowseForOpenFiles.
 * New function: MonitorFromRect.
 * GDI: Linux and macOS use same color format as Windows.
v0.973
 * Functions for getting and intercepting virtual key / keyboard states.
 * Functions for compositing LICE bitmaps into REAPER windows.
 * Enabled alpha blending for GDI blitting.
*/
void JS_ReaScriptAPI_Version(double* versionOut)
{
	*versionOut = 0.973;
}

void JS_Localize(const char* USEnglish, const char* LangPackSection, char* translationOut, int translationOut_sz)
{
	const char* trans = __localizeFunc(USEnglish, LangPackSection, 0);
	// Why not use NeedBig in this function?  Because seldom necessary, and I want to inform users about the "trick" to customize buffer size.
	/*
	size_t transLen = strlen(trans);
	bool reallocOK = false;
	if (realloc_cmd_ptr(&translationOutNeedBig, &translationOutNeedBig_sz, transLen))
	if (translationOutNeedBig && translationOutNeedBig_sz == transLen)
	reallocOK = true;
	if (reallocOK)
	memcpy(translationOutNeedBig, trans, transLen);
	else if (translationOutNeedBig && translationOutNeedBig_sz > 0)
	translationOutNeedBig[0] = 0;
	return;
	*/
	strncpy(translationOut, trans, translationOut_sz);
	translationOut[translationOut_sz - 1] = 0;
}

