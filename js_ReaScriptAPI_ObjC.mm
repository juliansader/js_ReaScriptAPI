#import <Cocoa/Cocoa.h>
#import <objc/objc-runtime.h>

bool JS_Window_SetOpacity_ObjC(void* hwnd, double alpha)
{
   if ([hwnd isKindOfClass:[NSView class]])
   {
      	NSView* view = (NSView*)hwnd;
	NSWindow* window = (NSWindow*)[view window];
	if (window)
	{
		CGFloat opacity = (CGFloat)alpha;
		[window setOpaque:NO];
		[window setWantsLayer:YES];
		[window setAlphaValue:opacity];
		[window setBackgroundColor:[NSColor clearColor]];
		//[window.layer setBackgroundColor:[NSColor clearColor]];
		return true;
	}
   }  
   return false;
}
