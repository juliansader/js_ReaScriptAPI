#import <Cocoa/Cocoa.h>
#import <objc/objc-runtime.h>

int JS_GetLevel(void* hwnd)
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
		return (int)[window level];
	else
		return 0;
}

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

bool JS_Window_SetZOrder_ObjC(void* hwnd, void* insertAfterHWND)
{
	NSView*   view = NULL; // Declare everything here, since can't do inside switch
	NSWindow* window = NULL;
	NSWindow* afterWindow = NULL;
	NSInteger afterWinNum = 0;
	
	if (hwnd) 
	{
		if ([(id)hwnd isKindOfClass:[NSView class]])
		{
			view   = (NSView*)hwnd;
			window = (NSWindow*)[view window]; // The view’s window object, if it is installed in a window.
		}
		else if ([(id)hwnd isKindOfClass:[NSWindow class]])
			window = (NSWindow*)hwnd;
	}
	
	if (window)
	{
		switch ((intptr_t)insertAfterHWND)
		{
			case -1: //HWND_TOPMOST:
				[window setLevel: NSFloatingWindowLevel];
				return true;
			case -2: //HWND_NOTOPMOST:
				[window setLevel: NSNormalWindowLevel];
				return true;
			case 0: //HWND_TOP:
				[window orderWindow:NSWindowAbove relativeTo:0];
				return true;
			case 1: //HWND_BELOW:
				[window setLevel: NSNormalWindowLevel];
				[window orderWindow:NSWindowBelow relativeTo:0];
				return true;
			default:
				if ([(id)insertAfterHWND isKindOfClass:[NSView class]])
				{
					view = (NSView*)insertAfterHWND;
					afterWindow = (NSWindow*)[view window]; // The view’s window object, if it is installed in a window.
				}
				else if ([(id)insertAfterHWND isKindOfClass:[NSWindow class]])
					afterWindow = (NSWindow*)insertAfterHWND;

				if (afterWindow)
				{
					afterWinNum = [afterWindow windowNumber];
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
