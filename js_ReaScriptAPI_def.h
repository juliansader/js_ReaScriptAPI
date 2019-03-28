#pragma once

#include "js_ReaScriptAPI.h"

// The macro and struct in this file are copied from SWS's ReaScript.cpp.)

// Macro to construct a comma-separated list of all the variants of a function name that are required for plugin_register(), and in the order required by the APIdef struct in which these variants are stored.
// APIFUNC(funcName) becomes (void*)funcName, "funcName", (void*)__vararg_funcName, "APIvararg_funcName", "API_funcName", "APIdef_funcName"
#define APIFUNC(x)  (void*)x,#x,(void*)__vararg_ ## x,"APIvararg_" #x "","API_" #x "","APIdef_" #x ""


// Struct to store info such as function name and help text for each function that the extensions intends to expose as API.
// This info will be used by REAPER's plugin_register functions to register the functions. 
// NOTE: REAPER requires the return values, paramaters and help text to be one long \0-separated string. 
//		The parm_names, parm_types and help field will there have to be concatenated before registration, and stored in the defstring field.
struct APIdef
{
	void* func; // pointer to the function that other extensions use
	const char* func_name;
	void* func_vararg; // pointer to the wrapper function that ReaScript API calls
	const char* regkey_vararg; // "APIvararg_funcName" - for 
	const char* regkey_func; // "API_funcName"
	const char* regkey_def; // "APIdef_funcName" 
	const char* ret_val; // return value type, as string
	const char* parm_types;
	const char* parm_names;
	const char* help;
	char* defstring; // For APIdef... Will be constructed and assigned while registering function

};


///////////////////////////////////////////////////////////////////////////////
//
// Add functions you want to export in the table below (+ related #include on
// top). To distinguish SWS and native functions, make sure function names have
// a prefix like "SWS_", "FNG_", etc.
// Your functions must be made dumb-proof, they must not trust any parameter.
// However, your do not need to ValidatePr() REAPER pointer parameters (such as 
// MediaTrack*, MediaItem*, etc): REAPER does this for you when a script runs
// an exported function.
// REAPER pointer parameters are validated against the prior ReaProject* param,
// (or the current project if absent/NULL). For ex., in the following function:
// void bla(ReaProject* p1, MediaTrack* t1, ReaProject* p2, MediaItem* i2),
// t1 will be validated against p1, but i2 will be validated against p2.
// Your must interpret NULL ReaProject* params as "the current project".
//
// When defining/documenting API function parameters:
//  - if a (char*,int) pair is encountered, name them buf, buf_sz
//  - if a (const char*,int) pair is encountered, buf, buf_sz as well
//  - if a lone basicType *, use varNameOut or varNameIn or
//    varNameInOptional (if last parameter(s))
// At the moment (REAPER v5pre6) the supported parameter types are:
//  - int, int*, bool, bool*, double, double*, char*, const char*
//  - AnyStructOrClass* (handled as an opaque pointer)
// At the moment (REAPER v5pre6) the supported return types are:
//  - int, bool, double, const char*
//  - AnyStructOrClass* (handled as an opaque pointer)
//
///////////////////////////////////////////////////////////////////////////////
/*
Julian remarks:

Under the hood, REAPER converts all ReaScript API functions to this standard format before calling the C++ function:
	void* func(void** arglist, int numparams)
In the REAPER IDE, the user doesn't need to type such a strange-looking vararg function.  Instead, the user can type standard Lua functions with

In the case of int, bool and void* parameters and return values, these values are stored in the void* variable itself.  

In the case of doubles, the sizeof(double) is larger than sizeof(void*), so cannot be stored in a void* parameter.  
		Instead, the void* argument is a pointer to the actual double value: *(double*)arglist[1]
		Similarly, a double return value should be stored in the *
		If you want to return a double value, don't return it as the C++ function's return value. Instead, include a "double* myVariableOut" in the arguments,


a more helpful version is presented
	to the user the user uses sees hepthe names and types of the parameters and return values that the user sees in the IDE are registered by plugin_register(APIdef_...).

So all functions must either be in this format, or get a wrapper function such as this:
		static void* __vararg_TEST_Window_GetRect(void** arglist, int numparms)
		{
			return (void*)(INT_PTR)TEST_Window_GetRect((void*)arglist[0], (int*)arglist[1], (int*)arglist[2], (int*)arglist[3], (int*)arglist[4]);
		}

*/

APIdef aAPIdefs[] =
{
	{ APIFUNC(JS_ReaScriptAPI_Version), "void", "double*", "versionOut", "Returns the version of the js_ReaScriptAPI extension.", },
	{ APIFUNC(JS_Localize), "void", "const char*,const char*,char*,int", "USEnglish,LangPackSection,translationOut,translationOut_sz", "Returns the translation of the given US English text, according to the currently loaded Language Pack.\n\nParameters:\n * LangPackSection: Language Packs are divided into sections such as \"common\" or \"DLG_102\".\n * In Lua, by default, text of up to 1024 chars can be returned. To increase (or reduce) the default buffer size, a string and size can be included as optional 3rd and 4th arguments.\n\nExample: reaper.JS_Localize(\"Actions\", \"common\", \"\", 20)", },

	{ APIFUNC(JS_VKeys_GetState), "bool", "char*,int*", "stateOutNeedBig,stateOutNeedBig_sz", "Retrieves the current states of all virtual keys, from 0x01 to 0xFF, in a 255-byte array.\n\nNote: Mouse buttons are not reliably detected, and JS_Mouse_GetState can be used instead.", },
	{ APIFUNC(JS_VKeys_GetHistory), "bool", "char*,int*", "stateOutNeedBig,stateOutNeedBig_sz", "Retrieves the cumulative states of all virtual keys since the last JS_VKeys_ClearHistory, in a 255-byte array.", },
	{ APIFUNC(JS_VKeys_ClearHistory), "void", "", "", "", },
	{ APIFUNC(JS_VKeys_Intercept), "int", "int,int", "keyCode,intercept", "Intercepting (blocking) virtual keys work similar to the native function PreventUIRefresh:  Each key has a (non-negative) intercept state, and the key is passed through as usual if the state equals 0, or blocked if the state is greater than 0.\n\nkeyCode: The virtual key code of the key, or -1 to change the state of all keys.\n\nintercept: A script can increase the intercept state by passing +1, or lower the state by passing -1.  Multiple scripts can block the same key, and the intercept state may reach up to 255. If zero is passed, the intercept state is not changed, but the current state is returned.\n\nReturns: If keyCode refers to a single key, the intercept state of that key is returned.  If keyCode = -1, the state of the key that is most strongly blocked (highest intercept state) is returned.", },

	{ APIFUNC(JS_WindowMessage_Post), "bool", "void*,const char*,int,int,int,int", "windowHWND,message,wParamLow,wParamHigh,lParamLow,lParamHigh", "Posts a message in the message queue associated with the thread that created the specified window, and returns without waiting.\n\nParameters:\n * message: String containing a single message name, such as \"WM_SETCURSOR\", or in hexadecimal format, \"0x0020\".\n (For a list of WM_ and CB_ message types that are valid cross-platform, refer to swell-types.h. Only these will be recognized by WM_ or CB_ name.)\n\nNotes:\n * For more information about parameter values, refer to documentation for the Win32 C++ function PostMessage.\n * Messages should only be sent to windows that were created from the main thread.\n * The message will be sent directly to the window, skipping interception by scripts.\n * Useful for simulating mouse clicks and calling mouse modifier actions from scripts.", },
	{ APIFUNC(JS_WindowMessage_Send), "int", "void*,const char*,int,int,int,int", "windowHWND,message,wParamLow,wParamHigh,lParamLow,lParamHigh", "Posts a message in the message queue associated with the thread that created the specified window, and returns without waiting.\n\nParameters:\n * message: String containing a single message name, such as \"WM_SETCURSOR\", or in hexadecimal format, \"0x0020\".\n (For a list of WM_ and CB_ message types that are valid cross-platform, refer to swell-types.h. Only these will be recognized by WM_ or CB_ name.)\n\nReturns:\n * Unlike JS_WindowMessage_Post, Send returns an LRESULT.\n\nNotes:\n * For more information about parameter and return values, refer to documentation for the Win32 C++ function SendMessage.\n * Messages should only be sent to windows that were created from the main thread.\n * The message will be sent directly to the window, skipping interception by scripts.\n * Useful for simulating mouse clicks and calling mouse modifier actions from scripts.", },
	{ APIFUNC(JS_WindowMessage_Peek), "bool", "void*,const char*,bool*,double*,int*,int*,int*,int*", "windowHWND,message,passedThroughOut,timeOut,wParamLowOut,wParamHighOut,lParamLowOut,lParamHighOut", "Polls the state of an intercepted message.\n\nParameters:\n * message: String containing a single message name, such as \"WM_SETCURSOR\", or in hexadecimal format, \"0x0020\".\n (For a list of WM_ and CB_ message types that are valid cross-platform, refer to swell-types.h. Only these will be recognized by WM_ or CB_ name.)\n\nReturns:\n * A retval of false indicates that the message type is not being intercepted in the specified window.\n * All messages are timestamped. A time of 0 indicates that no message if this type has been intercepted yet.\n * For more information about wParam and lParam for different message types, refer to Win32 C++ documentation.\n * For example, in the case of mousewheel, returns mousewheel delta, modifier keys, x position and y position.\n * wParamHigh, lParamLow and lParamHigh are signed, whereas wParamLow is unsigned.", },
	{ APIFUNC(JS_WindowMessage_Intercept), "int", "void*,const char*,bool", "windowHWND,message,passThrough", "Begins intercepting a window message type to specified window.\n\nParameters:\n * message: a single message type to be intercepted, either in WM_ or hexadecimal format. For example \"WM_SETCURSOR\" or \"0x0020\".\n * passThrough: Whether message should be blocked (false) or passed through (true) to the window.\n    For more information on message codes, refer to the Win32 C++ API documentation.\n    All WM_ and CB_ message types listed in swell-types.h should be valid cross-platform, and the function can recognize all of these by name. Other messages can be specified by their hex code.\n\nReturns:\n * 1: Success.\n * 0: The message type is already being intercepted by another script.\n * -2: message string could not be parsed.\n * -3: Failure getting original window process / window not valid.\n * -6: Could not obtain the window client HDC.\n\nNotes:\n * Intercepted messages can be polled using JS_WindowMessage_Peek.\n * Intercepted messages can be edited, if necessary, and then forwarded to their original destination using JS_WindowMessage_Post or JS_WindowMessage_Send.\n * To check whether a message type is being blocked or passed through, Peek the message type, or retrieve the entire List of intercepts.\n * Mouse events are typically received by the child window under the mouse, not the parent window.\nKeyboard events are usually *not* received by any individual window. To intercept keyboard events, use the VKey functions.", },
	{ APIFUNC(JS_WindowMessage_InterceptList), "int", "void*,const char*", "windowHWND,messages", "Begins intercepting window messages to specified window.\n\nParameters:\n * messages: comma-separated string of message types to be intercepted (either in WM_ or hexadecimal format), each with a \"block\" or \"passthrough\" modifier to specify whether the message should be blocked or passed through to the window. For example \"WM_SETCURSOR:block, 0x0201:passthrough\".\n    For more information on message codes, refer to the Win32 C++ API documentation.\n    All WM_ and CB_ message types listed in swell-types.h should be valid cross-platform, and the function can recognize all of these by name. Other messages can be specified by their hex code.\n\nReturns:\n * 1: Success.\n * 0: The message type is already being intercepted by another script.\n * -1: windowHWND is not a valid window.\n * -2: message string could not be parsed.\n * -3: Failure getting original window process.\n * -6: COuld not obtain the window client HDC.\n\nNotes:\n * Intercepted messages can be polled using JS_WindowMessage_Peek.\n * Intercepted messages can be edited, if necessary, and then forwarded to their original destination using JS_WindowMessage_Post or JS_WindowMessage_Send.\n * To check whether a message type is being blocked or passed through, Peek the message type, or retrieve the entire List of intercepts.", },
	{ APIFUNC(JS_WindowMessage_PassThrough), "int", "void*,const char*,bool", "windowHWND,message,passThrough", "Changes the passthrough setting of a message type that is already being intercepted.", },
	{ APIFUNC(JS_WindowMessage_ListIntercepts), "bool", "void*,char*,int", "windowHWND,listOutNeedBig,listOutNeedBig_sz", "Returns a string with a list of all message types currently being intercepted for the specified window.", },
	{ APIFUNC(JS_WindowMessage_Release), "int", "void*,const char*", "windowHWND,messages", "Release intercepts of specified message types.\n\nParameters:\n * messages: \"WM_SETCURSOR,WM_MOUSEHWHEEL\" or \"0x0020,0x020E\", for example.", },
	{ APIFUNC(JS_WindowMessage_ReleaseWindow), "void", "void*", "windowHWND", "Release script intercepts of window messages for specified window.", },
	{ APIFUNC(JS_WindowMessage_ReleaseAll), "void", "", "", "Release script intercepts of window messages for all windows.", },
	{ APIFUNC(JS_Window_OnCommand), "bool", "void*,int", "windowHWND,commandID", "Sends a \"WM_COMMAND\" message to the specified window, which simulates a user selecting a command in the window menu.\n\nThis function is similar to Main_OnCommand and MIDIEditor_OnCommand, but can send commands to any window that has a menu.\n\nIn the case of windows that are listed among the Action list's contexts (such as the Media Explorer), the commandIDs of the actions in the Actions list may be used.", },

	{ APIFUNC(JS_Window_GetParent), "void*", "void*", "windowHWND", "Retrieves a HWND to the specified window's parent or owner.\nReturns NULL if the window is unowned or if the function otherwise fails.", },
	{ APIFUNC(JS_Window_IsChild), "bool", "void*,void*", "parentHWND,childHWND", "Determines whether a window is a child window or descendant window of a specified parent window.", },
	{ APIFUNC(JS_Window_GetRelated), "void*", "void*,const char*", "windowHWND,relation", "Retrieves a handle to a window that has the specified relationship (Z-Order or owner) to the specified window.\nrelation: \"LAST\", \"NEXT\", \"PREV\", \"OWNER\" or \"CHILD\".\n(Refer to documentation for Win32 C++ function GetWindow.)", },
	{ APIFUNC(JS_Window_FindChildByID), "void*", "void*,int", "parentHWND,ID", "Similar to the C++ WIN32 function GetDlgItem, this function finds child windows by ID.\n\n(The ID of a window may be retrieved by JS_Window_GetLongPtr.)", },

	{ APIFUNC(JS_Window_SetFocus), "void", "void*", "windowHWND", "Sets the keyboard focus to the specified window.", },
	{ APIFUNC(JS_Window_GetFocus), "void*", "", "", "Retrieves a HWND to the window that has the keyboard focus, if the window is attached to the calling thread's message queue.", },
	{ APIFUNC(JS_Window_SetForeground), "void", "void*", "windowHWND", "Brings the specified window into the foreground, activates the window, and directs keyboard input to it.", },
	{ APIFUNC(JS_Window_GetForeground), "void*", "", "", "Retrieves a HWND to the foreground window (the window with which the user is currently working).", },

	{ APIFUNC(JS_Window_Enable), "void", "void*,bool", "windowHWND,enable", "Enables or disables mouse and keyboard input to the specified window or control.", },
	{ APIFUNC(JS_Window_Destroy), "void", "void*", "windowHWND", "Destroys the specified window.", },
	{ APIFUNC(JS_Window_Show), "void", "void*,const char*", "windowHWND,state", "Sets the specified window's show state.\n\nParameters:\n * state: Either \"SHOW\", \"SHOWNA\", \"SHOWMINIMIZED\",  or \"HIDE\".", },
	{ APIFUNC(JS_Window_IsVisible), "bool", "void*", "windowHWND", "Determines the visibility state of the window.", },
	{ APIFUNC(JS_Window_IsWindow), "bool", "void*", "windowHWND", "Determines whether the specified window handle identifies an existing window.\n\nOn macOS and Linux, only windows that were created by WDL/swell will be identified (and only such windows should be acted on by scripts).\n\nWARNING! On MacOS and Linux, REAPER WILL CRASH if the handle passed to a JS_Window or JS_WindowMessage function does not refer to an existing window.\n\nThe IsWindow function is slower on macOS and Linux. Preferably use native functions such as MIDIEditor_GetMode to test handles.", },

	{ APIFUNC(JS_Window_Resize), "void", "void*,int,int", "windowHWND,width,height", "Changes the dimensions of the specified window, keeping the top left corner position constant.\n * If resizing script GUIs, call gfx.update() after resizing.", },
	{ APIFUNC(JS_Window_Move), "void", "void*,int,int", "windowHWND,left,top", "Changes the position of the specified window, keeping its size constant.\n\nNOTES:\n * For top-level windows, position is relative to the primary display.\n * On Windows and Linux, position is calculated as the coordinates of the upper left corner of the window, relative to upper left corner of the primary display, and the positive Y-axis points downward.\n * On macOS, position is calculated as the coordinates of the bottom left corner of the window, relative to bottom left corner of the display, and the positive Y-axis points upward.\n * For a child window, on all platforms, position is relative to the upper-left corner of the parent window's client area.", },
	{ APIFUNC(JS_Window_SetPosition), "void", "void*,int,int,int,int", "windowHWND,left,top,width,height", "Sets the window position and size.", },
	{ APIFUNC(JS_Window_SetZOrder), "void", "void*,const char*,void*", "windowHWND,ZOrder,insertAfterHWND", "Sets the window Z order.\n\nParameters:\n * ZOrder: \"INSERT_AFTER\", \"BOTTOM\", \"TOPMOST\", \"NOTOPMOST\" or \"TOP\" ).\n * InsertAfterHWND: If ZOrder is INSERT_AFTER, insertAfterHWND must be a handle to the window to precede windowHWND in the Z order; otherwise, insertAfterHWND is ignored.", },
	{ APIFUNC(JS_Window_GetLongPtr), "void*", "void*,const char*", "windowHWND,info", "Returns information about the specified window.\n\ninfo: \"USERDATA\", \"WNDPROC\", \"DLGPROC\", \"ID\", \"EXSTYLE\" or \"STYLE\".\n\nFor documentation about the types of information returned, refer to the Win32 function GetWindowLongPtr.\n\nThe values returned by \"DLGPROC\" and \"WINPROC\" are typically used as-is, as pointers, whereas the others should first be converted to integers.\n\nIf the function fails, a null pointer is returned."},
	{ APIFUNC(JS_Window_GetLong), "void", "void*,const char*,double*", "windowHWND,info,retvalOut", "Similar to JS_Window_GetLongPtr, but returns the information as a number instead of a pointer. \n\nIn the case of \"DLGPROC\" and \"WINPROC\", the return values can be converted to pointers by JS_Window_HandleFromAddress.\n\nIf the function fails, the return value is 0." },
	{ APIFUNC(JS_Window_SetOpacity), "bool", "void*,const char*,double", "windowHWND,mode,value", "Sets the window opacity.\n\nParameters:\nmode: either \"ALPHA\" or \"COLOR\". \nvalue: If ALPHA, the specified value may range from zero to one, and will apply to the entire window. \nIf COLOR, value specifies a 0xRRGGBB color, and all pixels in this color will be made transparent. (All mouse clicks over transparent pixels will pass through, too).\n\nWARNING: COLOR mode is only available in Windows, not Linux or macOS.", },

	{ APIFUNC(JS_Window_GetTitle), "void", "void*,char*,int", "windowHWND,titleOut,titleOut_sz", "Returns the title (if any) of the specified window.", },
	{ APIFUNC(JS_Window_SetTitle), "bool", "void*,const char*", "windowHWND,title", "Changes the title of the specified window. Returns true if successful.", },
	{ APIFUNC(JS_Window_GetClassName), "void", "void*,char*,int", "windowHWND,classOut,classOut_sz", "WARNING: May not be fully implemented on macOS and Linux.", },

	{ APIFUNC(JS_Window_HandleFromAddress), "void*", "double", "address", "Converts an address to a handle (such as a HWND) that can be utilized by REAPER and other API functions.", },
	{ APIFUNC(JS_Window_AddressFromHandle), "void", "void*,double*", "handle,addressOut", "", },

};

///////////////////////////////////////////////////////////////////////////////
