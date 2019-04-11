#import <Cocoa/Cocoa.h>
#import <objc/objc-runtime.h>

bool JS_Window_SetOpacity_ObjC(void* hwnd, double alpha)
{
   //if ([(id)hwnd isKindOfClass:[NSWindow class]])
   //{
      NSWindow* window = (NSWindow*)hwnd;
      CGFloat opacity = (CGFloat)alpha;
      [window setAlphaValue:opacity];
      [window setBackgroundColor:[NSColor colorWithCalibratedWhite:1.0 alpha:opacity]];
      [window setOpaque:NO];
      return true;
   //}  
   //else
//		return false;
}
