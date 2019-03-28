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
		//		Load each of the undocumented functions.
		if (!((*(void **)&CoolSB_GetScrollInfo) = (void *)rec->GetFunc("CoolSB_GetScrollInfo")))
		{
			MessageBox(NULL, "Unable to import CoolSB_GetScrollInfo function.", "ERROR: js_ReaScriptAPI extension", 0);
			return 0;
		}
		if (!((*(void **)&AttachWindowTopmostButton) = (void *)rec->GetFunc("AttachWindowTopmostButton")))
		{
			MessageBox(NULL, "Unable to import AttachWindowTopmostButton function.", "ERROR: js_ReaScriptAPI extension", 0);
			return 0;
		}
		if (!((*(void **)&AttachWindowResizeGrip) = (void *)rec->GetFunc("AttachWindowResizeGrip")))
		{
			MessageBox(NULL, "Unable to import AttachWindowResizeGrip function.", "ERROR: js_ReaScriptAPI extension", 0);
			return 0;
		}
		if (!((*(void **)&CoolSB_SetScrollPos) = (void *)rec->GetFunc("CoolSB_SetScrollPos")))
		{
			MessageBox(NULL, "Unable to import CoolSB_SetScrollPos function.", "ERROR: js_ReaScriptAPI extension", 0);
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

		plugin_register("accelerator", &(Julian::sAccelerator));

		return 1; // success
	}
	// Does an extension need to do anything when unloading?  
	// To prevent memort leaks, perhaps try to delete any stuff that may remain in memory?
	// On Windows, LICE bitmaps are automatically destroyed when REAPER quits, but to make extra sure, this function will destroy them explicitly.
	// Why store stuff in extra sets?  For some unexplained reason REAPER crashes if I try to destroy LICE bitmaps explicitly. And for another unexplained reason, this roundabout way works...
	else 
	{
		std::set<HWND> windowsToRestore;
		for (auto& i : Julian::mapWindowData)
			windowsToRestore.insert(i.first);
		for (HWND hwnd : windowsToRestore)
			JS_WindowMessage_RestoreOrigProc(hwnd);

		std::set<LICE_IBitmap*> bitmapsToDelete;
		for (auto& i : Julian::LICEBitmaps)
			bitmapsToDelete.insert(i.first);
		for (LICE_IBitmap* bm : bitmapsToDelete)
			JS_LICE_DestroyBitmap(bm);

		for (auto& i : Julian::mapMallocToSize)
			free(i.first);

		plugin_register("-accelerator", &(Julian::sAccelerator));
		
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Virtual keys / Keyboard functions

static unsigned char VK_Bitmap[256] { 0 };
static unsigned char VK_BitmapHistory[256] { 0 };
static unsigned char VK_Intercepts[256] { 0 };
static constexpr size_t VK_Bitmap_sz_min1 = sizeof(VK_Bitmap)-1;

int JS_VKeys_Callback(MSG* event, accelerator_register_t*)
{
	const WPARAM& keycode = event->wParam;
	const UINT& uMsg = event->message;

	switch (uMsg) 
	{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (keycode < 256) {
				VK_Bitmap[keycode] = 1;
				if (VK_BitmapHistory[keycode] < 255) VK_BitmapHistory[keycode]++;
			}
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			if (keycode < 256)
				VK_Bitmap[keycode] = 0; // (keycode >> 3)] &= (~((uint8_t)(1 << (keycode & 0b00000111))));
			break;
	}

	if ((VK_Intercepts[keycode] != 0) && (uMsg != WM_KEYUP) && (uMsg != WM_SYSKEYUP)) // Block keystroke, but not when releasing key
		return 1; // Eat keystroke
	else
		return 0; // "Not my window", whatever this means?
}

void JS_VKeys_ClearHistory()
{
	std::fill_n(VK_BitmapHistory, 256, 0);
}

//void JS_VKeys_GetState(int* keys00to1FOut, int* keys20to3FOut, int* keys40to5FOut, int* keys60to7FOut, int* keys80to9FOut, int* keysA0toBFOut, int* keysC0toDFOut, int* keysE0toFFOut)
bool JS_VKeys_GetState(char* stateOutNeedBig, int stateOutNeedBig_sz)
{
	if (realloc_cmd_ptr(&stateOutNeedBig, &stateOutNeedBig_sz, VK_Bitmap_sz_min1)) {
		if (stateOutNeedBig_sz == VK_Bitmap_sz_min1) {
			memcpy(stateOutNeedBig, VK_Bitmap+1, VK_Bitmap_sz_min1);
			return true;
		}
	}
	return false;
}

bool JS_VKeys_GetHistory(char* stateOutNeedBig, int stateOutNeedBig_sz)
{
	if (realloc_cmd_ptr(&stateOutNeedBig, &stateOutNeedBig_sz, VK_Bitmap_sz_min1)) {
		if (stateOutNeedBig_sz == VK_Bitmap_sz_min1) {
			memcpy(stateOutNeedBig, VK_BitmapHistory+1, VK_Bitmap_sz_min1);
			return true;
		}
	}
	return false;
}

int JS_VKeys_Intercept(int keyCode, int intercept)
{
	if (0 <= keyCode && keyCode < 256) {
		if (intercept > 0 && VK_Intercepts[keyCode] < 255) VK_Intercepts[keyCode]++;
		else if (intercept < 0 && VK_Intercepts[keyCode] > 0) VK_Intercepts[keyCode]--;

		return VK_Intercepts[keyCode];
	}

	else if (keyCode == -1) {
		int maxIntercept = 0;
		if (intercept > 0) {
			for (int i = 0; i < 256; i++) {
				if (VK_Intercepts[i] < 255) VK_Intercepts[i]++;
				if (VK_Intercepts[i] > maxIntercept) maxIntercept = VK_Intercepts[i];
			}
		}
		else if (intercept < 0) {
			for (int i = 0; i < 256; i++) {
				if (VK_Intercepts[i] > 0) VK_Intercepts[i]--;
				if (VK_Intercepts[i] > maxIntercept) maxIntercept = VK_Intercepts[i];
			}
		}
		else { // intercept == 0
			for (int i = 0; i < 256; i++) {
				if (VK_Intercepts[i] > maxIntercept) maxIntercept = VK_Intercepts[i];
			}
		}
		return maxIntercept;
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////

bool JS_WindowMessage_ListIntercepts(void* windowHWND, char* listOutNeedBig, int listOutNeedBig_sz)
{
	using namespace Julian;
	listOutNeedBig[0] = '\0';
	
	if (mapWindowData.count((HWND)windowHWND))
	{
		auto& messages = mapWindowData[(HWND)windowHWND].mapMessages;
		for (const auto& it : messages)
		{
			if (strlen(listOutNeedBig) < (UINT)listOutNeedBig_sz - 32)
			{
				if (mapMsgToWM_.count(it.first))
					sprintf(strchr(listOutNeedBig, 0), "%s:", mapMsgToWM_[it.first].c_str());
				else
					sprintf(strchr(listOutNeedBig, 0), "0x%04X:", it.first);
				if ((it.second).passthrough)
					strcat(listOutNeedBig, "passthrough,\0");
				else
					strcat(listOutNeedBig, "block,\0");
			}
			else
				return false;
		}
		
		char* lastComma{ strrchr(listOutNeedBig, ',') }; // Remove final comma
		if (lastComma)
			*lastComma = '\0';
		return true;
	}
	else
		return true;
		
}

bool JS_WindowMessage_Post(void* windowHWND, const char* message, int wParamLow, int wParamHigh, int lParamLow, int lParamHigh)
{
	using namespace Julian;

	std::string msgString = message;
	UINT uMsg = 0;
	if (mapWM_toMsg.count(msgString))
		uMsg = mapWM_toMsg[msgString];
	else
	{
		errno = 0;
		char* endPtr;
		uMsg = strtoul(message, &endPtr, 16);
		if (endPtr == message || errno != 0) // 0x0000 is a valid message type, so cannot assume 0 is error.
			return false;
	}

	WPARAM wParam = MAKEWPARAM(wParamLow, wParamHigh);
	LPARAM lParam = MAKELPARAM(lParamLow, lParamHigh);
	HWND hwnd = (HWND)windowHWND;

	// Is this window currently being intercepted?
	if (mapWindowData.count(hwnd)) {
		sWindowData& w = mapWindowData[hwnd];
		if (w.mapMessages.count(uMsg)) {
			w.origProc(hwnd, uMsg, wParam, lParam); // WindowProcs usually return 0 if message was handled.  But not always, 
			return true;
		}
	}
	return !!PostMessage(hwnd, uMsg, wParam, lParam);
}


int JS_WindowMessage_Send(void* windowHWND, const char* message, int wParamLow, int wParamHigh, int lParamLow, int lParamHigh)
{
	using namespace Julian;

	std::string msgString = message;
	UINT uMsg = 0;
	if (mapWM_toMsg.count(msgString))
		uMsg = mapWM_toMsg[msgString];
	else
	{
		errno = 0;
		char* endPtr;
		uMsg = strtoul(message, &endPtr, 16);
		if (endPtr == message || errno != 0) // 0x0000 is a valid message type, so cannot assume 0 is error.
			return FALSE;
	}
	
	WPARAM wParam = MAKEWPARAM(wParamLow, wParamHigh);
	LPARAM lParam = MAKELPARAM(lParamLow, lParamHigh);

	return (int)SendMessage((HWND)windowHWND, uMsg, wParam, lParam);
}

// swell does not define these macros:
#ifndef GET_KEYSTATE_WPARAM
#define GET_KEYSTATE_WPARAM(wParam) LOWORD(wParam)
#endif
#ifndef GET_WHEEL_DELTA_WPARAM
#define GET_WHEEL_DELTA_WPARAM(wParam) ((short)HIWORD(wParam))
#endif


bool JS_Window_OnCommand(void* windowHWND, int commandID)
{
	return JS_WindowMessage_Post(windowHWND, "WM_COMMAND", commandID, 0, 0, 0);
}


bool JS_WindowMessage_Peek(void* windowHWND, const char* message, bool* passedThroughOut, double* timeOut, int* wParamLowOut, int* wParamHighOut, int* lParamLowOut, int* lParamHighOut)
{
	// lParamLow, lParamHigh, and wParamHigh are signed, whereas wParamLow is unsigned.
	using namespace Julian;

	std::string msgString = message;
	UINT uMsg = 0;
	if (mapWM_toMsg.count(msgString))
		uMsg = mapWM_toMsg[msgString];
	else
	{
		errno = 0;
		char* endPtr;
		uMsg = strtoul(message, &endPtr, 16);
		if (endPtr == message || errno != 0) // 0x0000 is a valid message type, so cannot assume 0 is error.
			return false;
	}

	if (mapWindowData.count((HWND)windowHWND))
	{
		sWindowData& w = mapWindowData[(HWND)windowHWND];

		if (w.mapMessages.count(uMsg))
		{
			sMsgData& m = w.mapMessages[uMsg];

			*passedThroughOut = m.passthrough;
			*timeOut		= m.time;
			*lParamLowOut	= GET_X_LPARAM(m.lParam);
			*lParamHighOut	= GET_Y_LPARAM(m.lParam);
			*wParamLowOut	= GET_KEYSTATE_WPARAM(m.wParam);
			*wParamHighOut	= GET_WHEEL_DELTA_WPARAM(m.wParam);

			return true;
		}
	}
	return false;
}

LRESULT CALLBACK JS_WindowMessage_Intercept_Callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	using namespace Julian;

	// If not in map, we don't know how to call original process.
	if (mapWindowData.count(hwnd) == 0)
		return 1;
	else {
		sWindowData& windowData = mapWindowData[hwnd]; // Get reference/alias because want to write into existing struct.

		// Event that should be intercepted? 
		if (windowData.mapMessages.count(uMsg)) // ".contains" has only been implemented in more recent C++ versions
		{
			windowData.mapMessages[uMsg].time = time_precise();
			windowData.mapMessages[uMsg].wParam = wParam;
			windowData.mapMessages[uMsg].lParam = lParam;

			// If event will not be passed through, can quit here.
			if (windowData.mapMessages[uMsg].passthrough == false)
			{
				// Most WM_ messages return 0 if processed, with only a few exceptions:
				switch (uMsg)
				{
				case WM_SETCURSOR:
				case WM_DRAWITEM:
				case WM_COPYDATA:
					return 1;
				case WM_MOUSEACTIVATE:
					return 3;
				default:
					return 0;
				}
			}
		}

		// Any other event that isn't intercepted.
		LRESULT r = windowData.origProc(hwnd, uMsg, wParam, lParam);

		if (uMsg == WM_PAINT) {
			for (auto& b : windowData.mapBitmaps) {
				sBitmapData& i = b.second;
#ifdef _WIN32
				AlphaBlend(windowData.windowDC, i.dstx, i.dsty, i.dstw, i.dsth, i.bitmapDC, i.srcx, i.srcy, i.srcw, i.srch, BLENDFUNCTION{ AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
#else
				StretchBlt(windowData.windowDC, i.dstx, i.dsty, i.dstw, i.dsth, i.bitmapDC, i.srcx, i.srcy, i.srcw, i.srch, SRCCOPY_USEALPHACHAN);
#endif
			}
		}

		return r;
	}
}


// Intercept a single message type
int JS_WindowMessage_Intercept(void* windowHWND, const char* message, bool passthrough)
{
	using namespace Julian;
	HWND hwnd = (HWND)windowHWND;
	UINT uMsg;

	// Convert string to UINT
	string msgString = message;
	if (mapWM_toMsg.count(msgString))
		uMsg = mapWM_toMsg[msgString];
	else
	{
		errno = 0;
		uMsg = strtoul(message, nullptr, 16);
		if (errno != 0 || (uMsg == 0 && !(strstr(message, "0x0000")))) // 0x0000 is a valid message type, so cannot assume 0 is error.
			return ERR_PARSING;
	}

	// Is this window already being intercepted?

	// Not yet intercepted, so create new sWindowdata map
	if (Julian::mapWindowData.count(hwnd) == 0) 
	{
		if (true) //!!!!! 
			return ERR_NOT_WINDOW;

		HDC windowDC = nullptr; //!!!!!(HDC)JS_GDI_GetClientDC(hwnd);
		if (!windowDC) 
			return ERR_WINDOW_HDC;

		// Try to get the original process.
		WNDPROC origProc = nullptr;
		#ifdef _WIN32
		origProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)JS_WindowMessage_Intercept_Callback);
		#else
		origProc = (WNDPROC)SetWindowLong(hwnd, GWL_WNDPROC, (LONG_PTR)JS_WindowMessage_Intercept_Callback);
		#endif
		if (!origProc) 
			return ERR_ORIGPROC;

		Julian::mapWindowData.emplace(hwnd, sWindowData{ origProc, windowDC }); // , map<UINT, sMsgData>{}, map<LICE_IBitmap*, sBitmapData>{} }); // Insert empty map
	}

	// Window already intercepted.  So try to add to existing maps.
	else
	{
		// Check that no overlaps: only one script may intercept each message type
		if (Julian::mapWindowData[hwnd].mapMessages.count(uMsg))
			return ERR_ALREADY_INTERCEPTED;
	}

	Julian::mapWindowData[hwnd].mapMessages.emplace(uMsg, sMsgData{ passthrough, 0, 0, 0 });
	return 1;
}

int JS_WindowMessage_PassThrough(void* windowHWND, const char* message, bool passThrough)
{
	using namespace Julian;
	HWND hwnd = (HWND)windowHWND;
	UINT uMsg;

	// Is this window already being intercepted?
	if (Julian::mapWindowData.count(hwnd) == 0)
		return ERR_ALREADY_INTERCEPTED; // Actually, NOT intercepted

	// Convert string to UINT
	string msgString = message;
	if (mapWM_toMsg.count(msgString))
		uMsg = mapWM_toMsg[msgString];
	else
	{
		errno = 0;
		uMsg = strtoul(message, nullptr, 16);
		if (errno != 0 || (uMsg == 0 && !(strstr(message, "0x0000")))) // 0x0000 is a valid message type, so cannot assume 0 is error.
			return ERR_PARSING;
	}

	// Is this message type actually already being intercepted?
	if (Julian::mapWindowData[hwnd].mapMessages.count(uMsg) == 0)
		return ERR_ALREADY_INTERCEPTED;

	// Change passthrough
	Julian::mapWindowData[hwnd].mapMessages[uMsg].passthrough = passThrough;

	return 1;
}

int JS_WindowMessage_InterceptList(void* windowHWND, const char* messages)
{
	using namespace Julian;
	HWND hwnd = (HWND)windowHWND;

	// strtok *replaces* characters in the string, so better copy messages to new char array.
	// It should not be possible for API functions to pass more than API_LEN characters.  But make doubly sure...
	if (strlen(messages) > API_LEN)
		return ERR_PARSING;
	char msg[API_LEN];
	strncpy(msg, messages, API_LEN);
	msg[API_LEN - 1] = 0;

	// messages string will be parsed into uMsg message types and passthrough modifiers 
	UINT uMsg;
	bool passthrough;

	// For use while tokenizing messages string
	char *token;
	std::string msgString;
	const char* delim = ":;,= \n\t";
	char* endPtr; 

	// Parsed info will be stored in these temporary maps
	map<UINT, sMsgData> newMessages;

	// Parse!
	token = strtok(msg, delim);
	while (token)
	{
		// Get message number
		msgString = token;
		if (mapWM_toMsg.count(msgString))
			uMsg = mapWM_toMsg[msgString];
		else
		{
			errno = 0;
			uMsg = strtoul(token, &endPtr, 16);
			if (endPtr == token || errno != 0) // 0x0000 is a valid message type, so cannot assume 0 is error.
				return ERR_PARSING;
		}

		// Now get passthrough
		token = strtok(NULL, delim);
		if (token == NULL)
			return ERR_PARSING; // Each message type must be followed by a modifier
		else if (token[0] == 'p' || token[0] == 'P')
			passthrough = true;
		else if (token[0] == 'b' || token[0] == 'B')
			passthrough = false;
		else // Not block or passthrough
			return ERR_PARSING;

		// Save in temporary maps
		newMessages.emplace(uMsg, sMsgData{ passthrough, 0, 0, 0 }); // time = 0 indicates that this message type is being intercepted OK, but that no message has yet been received.

		token = strtok(NULL, delim);
	}

	// Parsing went OK?  Any messages to intercept?
	if (newMessages.size() == 0)
		return ERR_PARSING;

	// Is this window already being intercepted?
	if (mapWindowData.count(hwnd) == 0) // Not yet intercepted
	{
		// IsWindow is slow in Linux and MacOS. 
		// However, checking may be prudent this may be necessary since Linux will crash if windowHWND is not an actual window.
		if (true) //!!!!!JS_Window_IsWindow(hwnd)) 
			return ERR_NOT_WINDOW;

		HDC windowDC = nullptr; //!!!!(HDC)JS_GDI_GetClientDC(hwnd);
		if (!windowDC) 
			return ERR_WINDOW_HDC;

		// Try to get the original process.
		WNDPROC origProc = nullptr;
#ifdef _WIN32
		origProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)JS_WindowMessage_Intercept_Callback);
#else
		origProc = (WNDPROC)SetWindowLong(hwnd, GWL_WNDPROC, (LONG_PTR)JS_WindowMessage_Intercept_Callback);
#endif
		if (!origProc) 
			return ERR_ORIGPROC;

		// Got everything OK.  Finally, store struct.
		Julian::mapWindowData.emplace(hwnd, sWindowData{ origProc, windowDC, newMessages }); // Insert into static map of namespace
		return 1;
	}

	// Already intercepted.  So add to existing maps.
	else
	{
		// Check that no overlaps: only one script may intercept each message type
		// Want to update existing map, so use aliases/references
		auto& existingMsg = Julian::mapWindowData[hwnd].mapMessages; // Messages that are already being intercepted for this window
		for (const auto& it : newMessages)
		{
			if (existingMsg.count(it.first)) // Oops, already intercepting this message type
			{
				return ERR_ALREADY_INTERCEPTED;
			}
		}
		// No overlaps, so add new intercepts to existing messages to intercept
		existingMsg.insert(newMessages.begin(), newMessages.end());
		return 1;
	}
}

int JS_WindowMessage_Release(void* windowHWND, const char* messages)
{
	using namespace Julian;
	HWND hwnd = (HWND)windowHWND;

	if (mapWindowData.count(hwnd) == 0)
		return ERR_NOT_WINDOW;

	// strtok *replaces* characters in the string, so better copy messages to new char array.
	// It should not be possible for API functions to pass more than API_LEN characters.  But make doubly sure...
	if (strlen(messages) > API_LEN)
		return ERR_PARSING;
	char msg[API_LEN];
	strncpy(msg, messages, API_LEN);
	msg[API_LEN - 1] = 0;

	// messages string will be parsed into uMsg message types and passthrough modifiers 
	UINT uMsg;
	char *token;
	std::string msgString;
	const char* delim = ":;,= \n\t";
	std::set<UINT> messagesToErase;

	// Parse!
	token = strtok(msg, delim);
	while (token)
	{
		// Get message number
		msgString = token;
		if (mapWM_toMsg.count(msgString))
			uMsg = mapWM_toMsg[msgString];
		else
			uMsg = strtoul(token, NULL, 16);
		if (!uMsg || (errno == ERANGE))
			return ERR_PARSING;

		// Store this parsed uMsg number
		messagesToErase.insert(uMsg);

		token = strtok(NULL, delim);
	}

	// Erase all message types that have been parsed
	auto& existingMessages = Julian::mapWindowData[hwnd].mapMessages; // Messages that are already being intercepted for this window
	for (const UINT& it : messagesToErase)
		existingMessages.erase(it);

	// If no messages need to be intercepted any more, release this window
	if ((existingMessages.size() == 0) && (Julian::mapWindowData[hwnd].mapBitmaps.size() == 0))
		JS_WindowMessage_RestoreOrigProc(hwnd);

	return TRUE;
}

void JS_WindowMessage_ReleaseAll()
{
	using namespace Julian;
	for (auto it = mapWindowData.begin(); it != mapWindowData.end(); ++it) {
		JS_WindowMessage_ReleaseWindow(it->first);
	}
}

void JS_WindowMessage_ReleaseWindow(void* windowHWND)
{
	using namespace Julian;
	HWND hwnd = (HWND)windowHWND;
	if (mapWindowData.count(hwnd)) {
		if (mapWindowData[hwnd].mapBitmaps.size() == 0) JS_WindowMessage_RestoreOrigProc(hwnd); // no linked bitmaps either, so can restore original WNDPROC
		else mapWindowData[hwnd].mapMessages.clear(); // delete intercepts, but leave linked bitmaps alone
	}
}

static void JS_WindowMessage_RestoreOrigProc(HWND hwnd)
{
	using namespace Julian;

	if (mapWindowData.count(hwnd)) {
		if (true) { //!!!!JS_Window_IsWindow(hwnd)) {
			WNDPROC origProc = mapWindowData[hwnd].origProc;
#ifdef _WIN32
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)origProc);
#else
			SetWindowLong(hwnd, GWL_WNDPROC, (LONG_PTR)origProc);
#endif
		}
		//JS_GDI_ReleaseDC(hwnd, mapWindowData[hwnd].windowDC);
		mapWindowData.erase(hwnd);
	}
}

static int  JS_WindowMessage_CreateNewMap(HWND hwnd)
{

}




//////////////////////////////////////////////////////////////////////////////////////////////////



void* JS_Window_GetParent(void* windowHWND)
{
	return GetParent((HWND)windowHWND);
}

bool  JS_Window_IsChild(void* parentHWND, void* childHWND)
{
	return !!IsChild((HWND)parentHWND, (HWND)childHWND);
}

void* JS_Window_GetRelated(void* windowHWND, const char* relation)
{
	/*
	#define GW_HWNDFIRST        0
	#define GW_HWNDLAST         1
	#define GW_HWNDNEXT         2
	#define GW_HWNDPREV         3
	#define GW_OWNER            4
	#define GW_CHILD            5
	*/
	int intMode;
	if		(strstr(relation, "FIRST"))	intMode = GW_HWNDFIRST;
	else if (strstr(relation, "LAST"))	intMode = GW_HWNDLAST;
	else if (strstr(relation, "NEXT"))	intMode = GW_HWNDNEXT;
	else if (strstr(relation, "PREV"))	intMode = GW_HWNDPREV;
	else if (strstr(relation, "OWNER"))	intMode = GW_OWNER;
	else if (strstr(relation, "CHILD"))	intMode = GW_CHILD;
	else return nullptr;
	return GetWindow((HWND)windowHWND, intMode);
}



void  JS_Window_SetFocus(void* windowHWND)
{
	// SWELL returns different types than Win32, so this function won't return anything.
	SetFocus((HWND)windowHWND);
}

void  JS_Window_SetForeground(void* windowHWND)
{
	// SWELL returns different types than Win32, so this function won't return anything.
	SetForegroundWindow((HWND)windowHWND);
}

void* JS_Window_GetFocus()
{
	return GetFocus();
}

void* JS_Window_GetForeground()
{
	return GetForegroundWindow();
}



void  JS_Window_Enable(void* windowHWND, bool enable)
{
	EnableWindow((HWND)windowHWND, (BOOL)enable); // (enable ? (int)1 : (int)0));
}

void  JS_Window_Destroy(void* windowHWND)
{
	DestroyWindow((HWND)windowHWND);
}

void  JS_Window_Show(void* windowHWND, const char* state)
{
	/*
	#define SW_HIDE 0
	#define SW_SHOWNA 1        // 8 on win32
	#define SW_SHOW 2          // 1 on win32
	#define SW_SHOWMINIMIZED 3 // 2 on win32

	// aliases (todo implement these as needed)
	#define SW_SHOWNOACTIVATE SW_SHOWNA
	#define SW_NORMAL SW_SHOW
	#define SW_SHOWNORMAL SW_SHOW
	#define SW_SHOWMAXIMIZED SW_SHOW
	#define SW_SHOWDEFAULT SW_SHOWNORMAL
	#define SW_RESTORE SW_SHOWNA
	*/
	int intState;
	if		(strstr(state, "SHOWNA"))	intState = SW_SHOWNA;
	else if (strstr(state, "MINI"))		intState = SW_SHOWMINIMIZED;
	else if (strstr(state, "HIDE"))		intState = SW_HIDE;
	else intState = SW_SHOW;
	ShowWindow((HWND)windowHWND, intState);
}

bool JS_Window_IsVisible(void* windowHWND)
{
	return !!IsWindowVisible((HWND)windowHWND);
}



void* JS_Window_SetCapture(void* windowHWND)
{
	return SetCapture((HWND)windowHWND);
}

void* JS_Window_GetCapture()
{
	return GetCapture();
}

void  JS_Window_ReleaseCapture()
{
	ReleaseCapture();
}


void* JS_Window_GetLongPtr(void* windowHWND, const char* info)
{
	int intMode;

#ifdef _WIN32
	if (strstr(info, "USER"))			intMode = GWLP_USERDATA;
	else if (strstr(info, "WND"))		intMode = GWLP_WNDPROC;
	else if (strstr(info, "ID"))		intMode = GWL_ID;
	else if (strstr(info, "EXSTYLE"))	intMode = GWL_EXSTYLE;
	else if (strstr(info, "STYLE"))		intMode = GWL_STYLE;
	else return nullptr;
	
	return (void*)GetWindowLongPtr((HWND)windowHWND, intMode);

#else 
	if (strstr(info, "USER"))			intMode = GWL_USERDATA;
	else if (strstr(info, "WND"))		intMode = GWL_WNDPROC;
	else if (strstr(info, "DLG"))		intMode = DWL_DLGPROC;
	else if (strstr(info, "ID"))		intMode = GWL_ID;
	else if (strstr(info, "EXSTYLE"))	intMode = GWL_EXSTYLE;
	else if (strstr(info, "STYLE"))		intMode = GWL_STYLE;
	else return nullptr;

	return (void*)GetWindowLong((HWND)windowHWND, intMode);
#endif
}

void JS_Window_GetLong(void* windowHWND, const char* info, double* retvalOut)
{
	int intMode;

#ifdef _WIN32
	if (strstr(info, "USER"))			intMode = GWLP_USERDATA;
	else if (strstr(info, "WND"))		intMode = GWLP_WNDPROC;
	else if (strstr(info, "ID"))		intMode = GWL_ID;
	else if (strstr(info, "EXSTYLE"))	intMode = GWL_EXSTYLE;
	else if (strstr(info, "STYLE"))		intMode = GWL_STYLE;
	else {*retvalOut = 0; return;}

	*retvalOut = (double)GetWindowLongPtr((HWND)windowHWND, intMode);

#else 
	if (strstr(info, "USER"))			intMode = GWL_USERDATA;
	else if (strstr(info, "WND"))		intMode = GWL_WNDPROC;
	else if (strstr(info, "DLG"))		intMode = DWL_DLGPROC;
	else if (strstr(info, "ID"))		intMode = GWL_ID;
	else if (strstr(info, "EXSTYLE"))	intMode = GWL_EXSTYLE;
	else if (strstr(info, "STYLE"))		intMode = GWL_STYLE;
	else {*retvalOut = 0; return;}

	*retvalOut = (double)GetWindowLong((HWND)windowHWND, intMode);
#endif
}


HWND JS_Window_FindEx(HWND parentHWND, HWND childHWND, const char* className, const char* title)
{
	// REAPER API cannot pass null pointers, so must do another way:
	HWND		c = ((parentHWND == childHWND) ? nullptr : childHWND);
	const char* t = ((strlen(title) == 0) ? nullptr : title);
	return FindWindowEx(parentHWND, c, className, t);
}


HWND JS_Window_FindChildByID(HWND parent, int ID)
{
	return GetDlgItem(parent, ID);
}

////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////

/*
Some functions using SetWindowLongPtr...

These flags are avilable in WDL/swell:

BEWARE, THESE NUMBERS ARE DIFFERENT FROM WIN32!!!

#define SWP_NOMOVE 1
#define SWP_NOSIZE 2
#define SWP_NOZORDER 4
#define SWP_NOACTIVATE 8
#define SWP_SHOWWINDOW 16
#define SWP_FRAMECHANGED 32
#define SWP_NOCOPYBITS 0
#define HWND_TOP        ((HWND)0)
#define HWND_BOTTOM     ((HWND)1)
#define HWND_TOPMOST    ((HWND)-1)
#define HWND_NOTOPMOST  ((HWND)-2)
*/
#ifndef SWP_NOOWNERZORDER
#define SWP_NOOWNERZORDER 0 // In WIN32, this would be 0x200, but in swell, use 0 so that no change when OR'ing.
#endif

// This function moves windows without resizing or requiring any info about window size.
void JS_Window_Move(void* windowHWND, int left, int top)
{
	SetWindowPos((HWND)windowHWND, NULL, left, top, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE);
}

// This function resizes windows without moving or requiring any info about window position.
void JS_Window_Resize(void* windowHWND, int width, int height)
{
	SetWindowPos((HWND)windowHWND, NULL, 0, 0, width, height, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE);
}

void JS_Window_SetPosition(void* windowHWND, int left, int top, int width, int height)
{
	SetWindowPos((HWND)windowHWND, NULL, left, top, width, height, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER );
}

void JS_Window_SetZOrder(void* windowHWND, const char* ZOrder, void* insertAfterHWND)
{
	HWND insertAfter;
	// Search for single chars that can distinguish the different ZOrder strings.
	if      (strstr(ZOrder, "IN"))		insertAfter = (HWND)insertAfterHWND; // insertAfter
	else if (strstr(ZOrder, "BO"))		insertAfter = HWND_BOTTOM;
	else if (strstr(ZOrder, "NOT"))		insertAfter = HWND_NOTOPMOST;
	else if (strstr(ZOrder, "TOPM"))	insertAfter = HWND_TOPMOST;
	else								insertAfter = HWND_TOP; // Top

	SetWindowPos((HWND)windowHWND, insertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}


bool JS_Window_SetOpacity(HWND windowHWND, const char* mode, double value)
{
	// Opacity can only be applied to top-level framed windows, AFAIK, and in Linux, REAPER crashes if opacity is applied to a child window.
	// So must check that style is WS_THICKFRAME.
	bool OK = false;
	if (JS_Window_IsWindow(windowHWND))
	{
#ifdef _WIN32
		if (GetWindowLongPtr(windowHWND, GWL_STYLE) & WS_THICKFRAME)
		{
			if (SetWindowLongPtr(windowHWND, GWL_EXSTYLE, GetWindowLongPtr(windowHWND, GWL_EXSTYLE) | WS_EX_LAYERED))
			{
				if (strchr(mode, 'A'))
					OK = !!(SetLayeredWindowAttributes(windowHWND, 0, (BYTE)(value * 255), LWA_ALPHA));
				else
				{
					UINT v = (UINT)value;
					OK = !!(SetLayeredWindowAttributes(windowHWND, (COLORREF)(((v & 0xFF0000) >> 16) | (v & 0x00FF00) | ((v & 0x0000FF) << 16)), 0, LWA_COLORKEY));
				}
#elif __linux__
		if (GetWindowLong(windowHWND, GWL_STYLE) & WS_THICKFRAME)
		{
			if (strchr(mode, 'A') || (!IsWindow(windowHWND)))
			{
				GdkWindow* w = (GdkWindow*)windowHWND->m_oswindow;
				if (w)
				{
					gdk_window_set_opacity(w, value);
					OK = true;
				}
#elif __APPLE__
		if (GetWindowLong(windowHWND, GWL_STYLE) & WS_THICKFRAME)
		{
			if (strchr(mode, 'A'))
			{
				OK = JS_Window_SetOpacity_ObjC(windowHWND, value);
#endif
			}
		}
	}
	return OK;
}


bool JS_Window_SetTitle(void* windowHWND, const char* title)
{
	return !!SetWindowText((HWND)windowHWND, title);
}

void JS_Window_GetTitle(void* windowHWND, char* titleOut, int titleOut_sz)
{
	GetWindowText((HWND)windowHWND, titleOut, titleOut_sz);
}

void JS_Window_GetClassName(HWND windowHWND, char* classOut, int classOut_sz)
{
	GetClassName(windowHWND, classOut, classOut_sz);
}

void* JS_Window_HandleFromAddress(double address)
{
	// Casting to intptr_t removes fractions and, in case of 32-bit systems, truncates too large addresses.
	//		This provides easy way to check whether address was valid.
	intptr_t intAddress = (intptr_t)address;
	if ((double)intAddress == address)
		return (void*)intAddress;
	else
		return nullptr;
}

void JS_Window_AddressFromHandle(void* handle, double* addressOut)
{
	*addressOut = (double)(intptr_t)handle;
}

//---------------------------------------------------------------------------------
// WDL/swell has not implemented IsWindow in Linux., and IsWindow is slow in MacOS.
// If scripts or extensions try to access a HWND that doesn't exist any more,
//		REAPER may crash completely.
// AFAIK, this implementation only searches WDL/swell windows, but this shouldn't be a problem,
//		since scripts are in any case only supposed to access REAPER's windows.
BOOL CALLBACK JS_Window_IsWindow_Callback_Child(HWND hwnd, LPARAM lParam)
{
	HWND& target = *(reinterpret_cast<HWND*>(lParam));
	if (hwnd == target)
	{
		target = nullptr;
		return FALSE;
	}
	else
		return TRUE;
}

BOOL CALLBACK JS_Window_IsWindow_Callback_Top(HWND hwnd, LPARAM lParam)
{
	HWND& target = *(reinterpret_cast<HWND*>(lParam));
	if (hwnd == target)
		target = nullptr;
	else
		EnumChildWindows(hwnd, JS_Window_IsWindow_Callback_Child, lParam);

	if (!target)
		return FALSE;
	else
		return TRUE;
}

bool  JS_Window_IsWindow(void* windowHWND)
{
#ifdef _WIN32
	return !!IsWindow((HWND)windowHWND);
#else
	// This implementation will enumerate all WDL/swell windows, looking for a match.
	// The "target" HWND will be passed to each callback function.
	// If a match is found, target will be replaced with NULL;
	if (!windowHWND) return false;
	HWND target = (HWND)windowHWND;
	/*bool isFloatingDock;

	if (!windowHWND)
		return false;
	else if (DockIsChildOfDock(target, &isFloatingDock) != -1)
		return true;
	*/

	HWND editor = MIDIEditor_GetActive();
	if (editor) {
		if (target == editor)
			return true;
		HWND midiview = GetDlgItem(editor, 1000);
		if (target == midiview) 
			return true;
		/*else {
			EnumChildWindows(editor, JS_Window_IsWindow_Callback_Child, reinterpret_cast<LPARAM>(&target));
			if (!target) return true;
		}*/
	}

	/*HWND main = GetMainHwnd();
	if (main) {
		if (target == main)
			return true;
		else {
			EnumChildWindows(main, JS_Window_IsWindow_Callback_Child, reinterpret_cast<LPARAM>(&target));
			if (!target) return true;
		}
	}*/

	EnumWindows(JS_Window_IsWindow_Callback_Top, reinterpret_cast<LPARAM>(&target));

	return !target;
#endif
}
				
/////////////////////////////////////////////////////////////////////////////////////////

void JS_LICE_DestroyBitmap(LICE_IBitmap* bitmap)
{
	using namespace Julian;
	// Also delete any occurence of this bitmap from UI Compositing
	if (LICEBitmaps.count(bitmap)) {
		for (auto& m : mapWindowData) {
			sWindowData& d = m.second;
			d.mapBitmaps.erase(bitmap);
		}
		LICEBitmaps.erase(bitmap);
		LICE__Destroy(bitmap);
	}
}
