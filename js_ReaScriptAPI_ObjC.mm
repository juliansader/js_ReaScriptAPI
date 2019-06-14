#import <Cocoa/Cocoa.h>
#import <objc/objc-runtime.h>

bool JS_Window_SetOpacity_ObjC(void* hwnd, double alpha)
{
   // For some reason, isKindOfClass doesn't work
   //if ([(id)hwnd isKindOfClass:[NSWindow class]])
   //{
      	NSWindow* window = (NSWindow*)hwnd;
      	CGFloat opacity = (CGFloat)alpha;
	[window setOpaque:NO];
	[window setWantsLayer:YES];
      	[window setAlphaValue:opacity];
	[window setBackgroundColor:[NSColor clearColor]];
	[window.layer setBackgroundColor:[NSColor clearColor]];
	return true;
   //}  
   //else
   // 	return false;
}
