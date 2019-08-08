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

bool JS_Window_SetZOrder_ObjC(void* hwnd, int order, void* insertAfterHWND)
{
	NSWindow* window = NULL;
	NSWindow* afterNSWindow = NULL;
	NSInteger afterWinNum = 0;
	
	if (hwnd) 
	{
		if ([(id)hwnd isKindOfClass:[NSView class]])
		{
			NSView* view = (NSView*)hwnd;
			window = (NSWindow*)[view window]; // The view’s window object, if it is installed in a window.
		}
		else if ([(id)hwnd isKindOfClass:[NSWindow class]])
			window = (NSWindow*)hwnd;
	}

	if (insertAfterHWND)
	{
		if ([(id)insertAfterHWND isKindOfClass:[NSView class]])
		{
			NSView* view = (NSView*)insertAfterHWND;
			afterNSWindow = (NSWindow*)[view window]; // The view’s window object, if it is installed in a window.
		}
		else if ([(id)insertAfterHWND isKindOfClass:[NSWindow class]])
			afterNSWindow = (NSWindow*)insertAfterHWND;
	}
	
	if (window)
	{
		switch (order)
		{
			case HWND_TOPMOST:
				[window setLevel: NSFloatingWindowLevel];
				return true;
			case HWND_NOTOPMOST:
				[window setLevel: NSNormalWindowLevel];
				return true;
			case HWND_TOP:
				[window orderWindow:NSWindowAbove relativeTo:0];
				return true;
			case HWND_BELOW:
				[window setLevel: NSNormalWindowLevel];
				[window orderWindow:NSWindowBelow relativeTo:0];
				return true;
			default:
				if (afterNSWindow)
				{
					afterWinNum = [afterNSWindow windowNumber];
					if (afterWinNum)
					{
						[window orderWindow:NSWindowAbove relativeTo:afterWinNum];
						return true;
					}
				}
		}
	}
   	return false;
}
