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

};

///////////////////////////////////////////////////////////////////////////////
