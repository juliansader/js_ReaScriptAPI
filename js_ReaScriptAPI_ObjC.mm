/*
#ifndef SWELL_TARGET_OSX
#define SWELL_TARGET_OSX 1
#endif

#ifndef __OBJC__
#define __OBJC__ 1
#endif

#ifdef SWELL_NO_METAL
#error "SWELL_NO_METAL should be defined for the entire project"
#endif

*/
#import <Cocoa/Cocoa.h>
#import <objc/objc-runtime.h>

/*
#include "./WDL/swell/swell-types.h"
//#include "./WDL/swell/swell-internal.h"

typedef struct WindowPropRec
{
  char *name; // either <64k or a strdup'd name
  void *data;
  struct WindowPropRec *_next;
} WindowPropRec;

@interface SWELL_hwndChild : NSView // <NSDraggingSource>
{
@public
  int m_enabled; // -1 if preventing focus
  DLGPROC m_dlgproc;
  WNDPROC m_wndproc;
  LONG_PTR m_userdata;
  LONG_PTR m_extradata[32];
  NSInteger m_tag;
  int m_isfakerightmouse;
  char m_hashaddestroy; // 2 = WM_DESTROY has finished completely
  HMENU m_menu;
  BOOL m_flip;
  bool m_supports_ddrop;
  bool m_paintctx_used;
  HDC m_paintctx_hdc;
  WindowPropRec *m_props;
  NSRect m_paintctx_rect;
  BOOL m_isopaque;
  char m_titlestr[1024];
  unsigned int m_create_windowflags;
  NSOpenGLContext *m_glctx;
  char m_isdirty; // &1=self needs redraw, &2=children may need redraw
  char m_allow_nomiddleman;
  id m_lastTopLevelOwner; // save a copy of the owner, if any
  id m_access_cacheptrs[6];
  const char *m_classname;

  char m_use_metal; // 1=normal mode, 2=full pipeline (GetDC() etc support). -1 is for non-metal async layered mode. -2 for non-metal non-async layered

  // metal state (if used)
  char m_metal_dc_dirty;  // used to track state during paint or getdc/releasedc. set to 1 if dirty, 2 if GetDC() but no write yet
  char m_metal_gravity; // &1=resizing left, &2=resizing top
  bool m_metal_retina; // last-retina-state, triggered to true by StretchBlt() with a 2:1 ratio

  bool m_metal_in_needref_list;
  RECT m_metal_in_needref_rect; 
  NSRect m_metal_lastframe;

  id m_metal_texture; // id<MTLTexture> -- owned if in full pipeline mode, otherwise reference to m_metal_drawable
  id m_metal_pipelineState; // id<MTLRenderPipelineState> -- only used in full pipeline mode
  id m_metal_commandQueue; // id<MTLCommandQueue> -- only used in full pipeline mode
  id m_metal_drawable; // id<CAMetalDrawable> -- only used in normal mode
  id m_metal_device; // id<MTLDevice> -- set to last-used-device
  DWORD m_metal_device_lastchkt;

}
- (id)initChild:(SWELL_DialogResourceIndex *)resstate Parent:(NSView *)parent dlgProc:(DLGPROC)dlgproc Param:(LPARAM)par;
- (LRESULT)onSwellMessage:(UINT)msg p1:(WPARAM)wParam p2:(LPARAM)lParam;
-(HANDLE)swellExtendedDragOp:(id <NSDraggingInfo>)sender retGlob:(BOOL)retG;
- (const char *)onSwellGetText;
-(void)onSwellSetText:(const char *)buf;
-(LONG)swellGetExtendedStyle;
-(void)swellSetExtendedStyle:(LONG)st;
-(HMENU)swellGetMenu;
-(BOOL)swellHasBeenDestroyed;
-(void)swellSetMenu:(HMENU)menu;
-(LONG_PTR)getSwellUserData;
-(void)setSwellUserData:(LONG_PTR)val;
-(void)setOpaque:(bool)isOpaque;
-(LPARAM)getSwellExtraData:(int)idx;
-(void)setSwellExtraData:(int)idx value:(LPARAM)val;
-(void)setSwellWindowProc:(WNDPROC)val;
-(WNDPROC)getSwellWindowProc;
-(void)setSwellDialogProc:(DLGPROC)val;
-(DLGPROC)getSwellDialogProc;

- (NSArray*) namesOfPromisedFilesDroppedAtDestination:(NSURL*)droplocation;

-(void) getSwellPaintInfo:(PAINTSTRUCT *)ps;
- (int)swellCapChangeNotify;
-(unsigned int)swellCreateWindowFlags;

-(bool)swellCanPostMessage;
-(int)swellEnumProps:(PROPENUMPROCEX)proc lp:(LPARAM)lParam;
-(void *)swellGetProp:(const char *)name wantRemove:(BOOL)rem;
-(int)swellSetProp:(const char *)name value:(void *)val ;
-(NSOpenGLContext *)swellGetGLContext;
- (void) setEnabledSwellNoFocus;
-(const char *)getSwellClass;

// NSAccessibility

// attribute methods
//- (NSArray *)accessibilityAttributeNames;
- (id)accessibilityAttributeValue:(NSString *)attribute;
//- (BOOL)accessibilityIsAttributeSettable:(NSString *)attribute;
//- (void)accessibilitySetValue:(id)value forAttribute:(NSString *)attribute;

// parameterized attribute methods
//- (NSArray *)accessibilityParameterizedAttributeNames;
//- (id)accessibilityAttributeValue:(NSString *)attribute forParameter:(id)parameter;

// action methods
//- (NSArray *)accessibilityActionNames;
//- (NSString *)accessibilityActionDescription:(NSString *)action;
//- (void)accessibilityPerformAction:(NSString *)action;

// Return YES if the UIElement doesn't show up to the outside world - i.e. its parent should return the UIElement's children as its own - cutting the UIElement out. E.g. NSControls are ignored when they are single-celled.
- (BOOL)accessibilityIsIgnored;

// Returns the deepest descendant of the UIElement hierarchy that contains the point. You can assume the point has already been determined to lie within the receiver. Override this method to do deeper hit testing within a UIElement - e.g. a NSMatrix would test its cells. The point is bottom-left relative screen coordinates.
- (id)accessibilityHitTest:(NSPoint)point;

// Returns the UI Element that has the focus. You can assume that the search for the focus has already been narrowed down to the reciever. Override this method to do a deeper search with a UIElement - e.g. a NSMatrix would determine if one of its cells has the focus.
- (id)accessibilityFocusedUIElement;

-(void) swellOnControlDoubleClick:(id)sender;

#ifdef MAC_OS_X_VERSION_10_8
// for radio button with the OSX 10.8+ SDK, see comment in SWELL_MakeControl
-(void) onSwellCommand0:(id)sender;
-(void) onSwellCommand2:(id)sender;
-(void) onSwellCommand3:(id)sender;
-(void) onSwellCommand4:(id)sender;
-(void) onSwellCommand5:(id)sender;
-(void) onSwellCommand6:(id)sender;
-(void) onSwellCommand7:(id)sender;
#endif

-(BOOL) swellWantsMetal;
-(void) swellDrawMetal:(const RECT *)forRect;
@end
*/
	
void* JS_GetContentViewFromSwellHWND(void* hwnd)
{
	if (hwnd && [(id)hwnd isKindOfClass:[NSView class]])
		return [[(NSView*)hwnd window] contentView];
	else if (hwnd && [(id)hwnd isKindOfClass:[NSWindow class]])
		return [[(NSWindow*)hwnd window] contentView];	
	else
		return NULL;
}

// The NSWindow is the container of the NSView child windows.
void* JS_GetNSWindowFromSwellHWND(void* hwnd)
{
	NSWindow* window = NULL;
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
	return window;
}

bool JS_Window_SetOpacity_ObjC(void* hwnd, double alpha)
{
	NSWindow* window = (NSWindow*)JS_GetNSWindowFromSwellHWND(hwnd);
	
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

// Get the NSWindowLevel of a swell HWND.
int JS_GetLevel_ObjC(void* hwnd)
{
	NSWindow* window = (NSWindow*)JS_GetNSWindowFromSwellHWND(hwnd);
	
	if (window)
		return (int)[window level];
	else
		return 0;
}

// swell doesn't provide full functionality for SetWindowPos. In particular, TOPMOST and NOTOPMOST aren't implemented.
// So I have tried to code my own.
// This was necessary for JS_Window_Create, since swell automatically gives the window a level of 0, which is below REAPER's floating windows.
bool JS_Window_SetZOrder_ObjC(void* hwnd, void* insertAfterHWND)
/*
	Here are some of the standard macOS window levels:
	backstopMenu: -20
	normalWindow: 0
	floatingWindow: 3
	tornOffMenuWindow: 3
	modalPanelWindow: 8
	utilityWindow: 19
	dockWindow: 20
	mainMenuWindow: 24
	statusWindow: 25
	popUpMenuWindow: 101
	overlayWindow: 102
	
	REAPER uses a non-standard level for its floating windows, namely 1, 
		which is just slightly above the main window at 0 = NSNormalWindowLevel.
*/
{
	NSWindow* window = (NSWindow*)JS_GetNSWindowFromSwellHWND(hwnd);
	
	if (window)
	{
		NSWindow* afterThisWindow = NULL; // Declare here, since can't inside switch
		
		switch ((intptr_t)insertAfterHWND)
		{
			case -1: //HWND_TOPMOST:
				[window setLevel: NSFloatingWindowLevel]; // = 3
				return true;
			case -2: //HWND_NOTOPMOST:
				[window setLevel: 1]; // Standard level for REAPER's floating windows
				return true;
			case 0: //HWND_TOP:
				if ([window level] < 1)
					[window setLevel: 1];
				[window orderWindow:NSWindowAbove relativeTo:0]; // Bring to top within current level
				return true;
			case 1: //HWND_BELOW:
				[window setLevel: NSNormalWindowLevel]; // Below REAPER's floating windoww, same level as main
				[window orderWindow:NSWindowBelow relativeTo:0];
				return true;
			default: // insertAfter is a target window
				afterThisWindow = (NSWindow*)JS_GetNSWindowFromSwellHWND(insertAfterHWND);
				if (afterThisWindow)
				{
					[window setLevel: [afterThisWindow level]];
					[window orderWindow:NSWindowBelow relativeTo:[afterThisWindow windowNumber]];
					return true;
				}
		}
	}
   	return false;
}

/*
int JS_GetMetalMode(void* hwnd)
{
	if (!hwnd) return -10;
	if (![(id)hwnd isKindOfClass:[SWELL_hwndChild class]]) return -11;
	SWELL_hwndChild *ch = (SWELL_hwndChild *)hwnd;
  	return ch->m_use_metal;
}
*/
