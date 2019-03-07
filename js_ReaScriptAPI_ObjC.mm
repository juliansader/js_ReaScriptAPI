#import <Cocoa/Cocoa.h>
#import <objc/objc-runtime.h>

int JS_Window_SetOpacity_ObjC(HWND hwnd, double alpha)
{
   if ([(id)hwnd isKindOfClass:[NSWindow class]])
   {
      NSWindow *window = (NSWindow *)hwnd;
      CGFloat opacity = alpha;
      [window setAlphaValue:opacity];
      [window setOpaque:NO];
	  return 1;
   }  
   else
		return 0;
}
