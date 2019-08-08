#import <Cocoa/Cocoa.h>
#import <objc/objc-runtime.h>

bool JS_Window_SetOpacity_ObjC(void* hwnd, double alpha)
{
	NSWindow* window = NULL;
   	if ([(id)hwnd isKindOfClass:[NSView class]])
	{
		NSView* view = (NSView*)hwnd;
		window = (NSWindow*)[view window]; // The view’s window object, if it is installed in a window.
	}
	else if ([(id)hwnd isKindOfClass:[NSWindow class]])
		window = (NSWindow*)hwnd;
	
	if (window)
	{
		CGFloat opacity = (CGFloat)alpha;
		[window setOpaque:NO];
		//[window setWantsLayer:YES];
		[window setAlphaValue:opacity];
		//[window setBackgroundColor:[NSColor clearColor]];
		//[window.layer setBackgroundColor:[NSColor clearColor]];
		return true;
	}
	else
		return false;
}

bool JS_Window_SetZOrder_ObjC(void* hwnd, int order)
{
	NSWindow* window = NULL;
	if ([(id)hwnd isKindOfClass:[NSView class]])
	{
		NSView* view = (NSView*)hwnd;
		window = (NSWindow*)[view window]; // The view’s window object, if it is installed in a window.
	}
	else if ([(id)hwnd isKindOfClass:[NSWindow class]])
		window = (NSWindow*)hwnd;

	if (window)
	{
		if (order == 1)
		{
			[window setLevel: NSScreenSaverWindowLevel];
			[window makeKeyAndOrderFront:NULL];
			return true;
		}
	}
   	return false;
}
