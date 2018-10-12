reaper_js_ReaScriptAPI: s_ReaScriptAPI.cpp swell-modstub-generic.cpp
    g++ -fPIC -shared js_ReaScriptAPI.cpp swell-modstub-generic.cpp -I. -std=gnu++11 -Wall -m32 -o reaper_js_ReaScriptAPI324.so
