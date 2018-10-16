#import <Cocoa/Cocoa.h>
#import <objc/objc-runtime.h>
#include "swell.h"

void JS_Window_SetTransparency_ObjC(void* hwnd, double alpha)
{
   if ([(id)hwnd isKindOfClass:[NSWindow class]])
   {
      NSWindow *window = (NSWindow *)hwnd;
      CGFloat opacity = alpha;
      [window setAlphaValue:opacity];
      [window setOpaque:NO];
   }  
}
