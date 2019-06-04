#import <Cocoa/Cocoa.h>
#import <objc/objc-runtime.h>

bool JS_Window_SetOpacity_ObjC(void* hwnd, double alpha)
{
   // For some reason, isKindOfClass doesn't work
   //if ([(id)hwnd isKindOfClass:[NSWindow class]])
   //{
      NSWindow* window = (NSWindow*)hwnd;
      CGFloat opacity = (CGFloat) alpha;
      [window setAlphaValue:opacity];
      [window setOpaque:NO];
      [window setBackgroundColor:[NSColor clearColor]];
      [window setIgnoresMouseEvents:NO];
	  return true;
   //}  
   //else
   // 	return false;
}
