#include <simply_gui/gui_cocoa.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#import <Cocoa/Cocoa.h>

#include <zlay_shell.h>
#include <zlay_ui.h>

static bool ZLay_CocoaRectContains(ZLay_Rect rect, NSPoint point);
static void ZLay_CocoaRenderCommands(ZLay_CocoaButtonDemo* demo, ZLay_RenderCommandList commands);
static ZLay_RenderCommandList ZLay_CocoaBuildButtonLayout(ZLay_CocoaButtonDemo* demo);
static void ZLay_CocoaRedraw(ZLay_CocoaButtonDemo* demo);
static void ZLay_CocoaUpdateHover(ZLay_CocoaButtonDemo* demo, NSPoint point);

@interface ZLayCocoaWindowDelegate : NSObject <NSWindowDelegate> {
@public
  ZLay_CocoaButtonDemo* demo;
}
- (instancetype)initWithDemo:(ZLay_CocoaButtonDemo*)buttonDemo;
@end

@implementation ZLayCocoaWindowDelegate
- (instancetype)initWithDemo:(ZLay_CocoaButtonDemo*)buttonDemo {
  self = [super init];
  if (self != nil) {
    demo = buttonDemo;
  }
  return self;
}

- (void)windowWillClose:(NSNotification*)notification {
  (void)notification;
  if (demo != NULL) {
    demo->running = false;
  }
}
@end

@interface ZLayCocoaButtonView : NSView {
@public
  ZLay_CocoaButtonDemo* demo;
  NSTrackingArea* trackingArea;
}
- (instancetype)initWithFrame:(NSRect)frameRect demo:(ZLay_CocoaButtonDemo*)buttonDemo;
@end

@implementation ZLayCocoaButtonView
- (instancetype)initWithFrame:(NSRect)frameRect demo:(ZLay_CocoaButtonDemo*)buttonDemo {
  self = [super initWithFrame:frameRect];
  if (self != nil) {
    demo = buttonDemo;
    trackingArea = nil;
  }
  return self;
}

- (void)dealloc {
  if (trackingArea != nil) {
    [self removeTrackingArea:trackingArea];
    [trackingArea release];
  }
  [super dealloc];
}

- (BOOL)isFlipped {
  return YES;
}

- (BOOL)acceptsFirstResponder {
  return YES;
}

- (void)updateTrackingAreas {
  [super updateTrackingAreas];
  if (trackingArea != nil) {
    [self removeTrackingArea:trackingArea];
    [trackingArea release];
  }

  trackingArea = [[NSTrackingArea alloc]
    initWithRect:[self bounds]
    options:NSTrackingMouseMoved | NSTrackingMouseEnteredAndExited | NSTrackingActiveInKeyWindow | NSTrackingInVisibleRect
    owner:self
    userInfo:nil
  ];
  [self addTrackingArea:trackingArea];
}

- (void)drawRect:(NSRect)dirtyRect {
  (void)dirtyRect;
  if (demo == NULL || demo->ctx == NULL) {
    return;
  }
  demo->width = (int)[self bounds].size.width;
  demo->height = (int)[self bounds].size.height;
  ZLay_RenderCommandList commands = ZLay_CocoaBuildButtonLayout(demo);
  ZLay_CocoaRenderCommands(demo, commands);
}

- (void)mouseMoved:(NSEvent*)event {
  ZLay_CocoaUpdateHover(demo, [self convertPoint:[event locationInWindow] fromView:nil]);
}

- (void)mouseDragged:(NSEvent*)event {
  ZLay_CocoaUpdateHover(demo, [self convertPoint:[event locationInWindow] fromView:nil]);
}

- (void)mouseExited:(NSEvent*)event {
  (void)event;
  if (demo != NULL && (demo->hovered || demo->pressed)) {
    demo->hovered = false;
    demo->pressed = false;
    ZLay_CocoaRedraw(demo);
  }
}

- (void)mouseDown:(NSEvent*)event {
  NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
  if (demo != NULL && ZLay_CocoaRectContains(demo->button_bounds, point)) {
    demo->pressed = true;
    demo->hovered = true;
    ZLay_CocoaRedraw(demo);
  }
}

- (void)mouseUp:(NSEvent*)event {
  NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
  if (demo != NULL) {
    demo->pressed = false;
    demo->hovered = ZLay_CocoaRectContains(demo->button_bounds, point);
    ZLay_CocoaRedraw(demo);
  }
}

- (void)keyDown:(NSEvent*)event {
  if ([[event charactersIgnoringModifiers] isEqualToString:@"\033"]) {
    [[self window] close];
  } else {
    [super keyDown:event];
  }
}
@end

static bool ZLay_CocoaRectContains(ZLay_Rect rect, NSPoint point) {
  return (float)point.x >= rect.x &&
         (float)point.y >= rect.y &&
         (float)point.x < rect.x + rect.width &&
         (float)point.y < rect.y + rect.height;
}

static NSColor* ZLay_CocoaColor(ZLay_Color color) {
  return [NSColor colorWithCalibratedRed:(CGFloat)color.r / 255.0
                                   green:(CGFloat)color.g / 255.0
                                    blue:(CGFloat)color.b / 255.0
                                   alpha:(CGFloat)color.a / 255.0];
}

static NSString* ZLay_CocoaStringFromZLay(ZLay_String text) {
  if (text.chars == NULL || text.length == 0u) {
    return [[NSString alloc] initWithString:@""];
  }

  NSString* string = [[NSString alloc] initWithBytes:text.chars
                                              length:text.length
                                            encoding:NSUTF8StringEncoding];
  if (string == nil) {
    string = [[NSString alloc] initWithBytes:text.chars
                                      length:text.length
                                    encoding:NSISOLatin1StringEncoding];
  }
  return string;
}

static ZLay_Dimensions ZLay_CocoaMeasureText(ZLay_String text, void* user) {
  ZLay_CocoaButtonDemo* demo = (ZLay_CocoaButtonDemo*)user;
  NSFont* font = demo != NULL && demo->font != NULL ? (NSFont*)demo->font : [NSFont systemFontOfSize:14.0];
  NSString* string = ZLay_CocoaStringFromZLay(text);
  NSDictionary* attributes = [NSDictionary dictionaryWithObject:font forKey:NSFontAttributeName];
  NSSize size = [string sizeWithAttributes:attributes];
  [string release];
  return ZLay_Dimensions{(float)size.width, (float)size.height};
}

static void ZLay_CocoaDrawRect(const ZLay_RenderCommand* command) {
  NSRect rect;

  if (command->color.a == 0u) {
    return;
  }

  rect = NSMakeRect(command->bounds.x, command->bounds.y, command->bounds.width, command->bounds.height);
  [ZLay_CocoaColor(command->color) setFill];

  if (command->radius > 0.0f) {
    NSBezierPath* path = [NSBezierPath bezierPathWithRoundedRect:rect
                                                         xRadius:(CGFloat)command->radius
                                                         yRadius:(CGFloat)command->radius];
    [path fill];
  } else {
    NSRectFill(rect);
  }
}

static void ZLay_CocoaDrawText(ZLay_CocoaButtonDemo* demo, const ZLay_RenderCommand* command) {
  NSFont* font;
  NSColor* color;
  NSString* string;
  NSDictionary* attributes;
  NSSize size;
  CGFloat y;

  if (command->text.chars == NULL || command->text.length == 0u) {
    return;
  }

  font = demo != NULL && demo->font != NULL ? (NSFont*)demo->font : [NSFont systemFontOfSize:14.0];
  color = ZLay_CocoaColor(command->color);
  string = ZLay_CocoaStringFromZLay(command->text);
  attributes = [NSDictionary dictionaryWithObjectsAndKeys:
    font, NSFontAttributeName,
    color, NSForegroundColorAttributeName,
    nil
  ];
  size = [string sizeWithAttributes:attributes];
  y = (CGFloat)command->bounds.y + (((CGFloat)command->bounds.height - size.height) * 0.5);

  [string drawAtPoint:NSMakePoint((CGFloat)command->bounds.x, y) withAttributes:attributes];
  [string release];
}

static void ZLay_CocoaRenderCommands(ZLay_CocoaButtonDemo* demo, ZLay_RenderCommandList commands) {
  demo->button_bounds = ZLay_Rect{};

  for (uint32_t i = 0; i < commands.count; ++i) {
    const ZLay_RenderCommand* command = &commands.commands[i];

    if (command->type == ZLAY_CMD_RECT) {
      const bool root_rect =
        command->bounds.x == 0.0f &&
        command->bounds.y == 0.0f &&
        command->bounds.width == (float)demo->width &&
        command->bounds.height == (float)demo->height;

      if (!root_rect) {
        demo->button_bounds = command->bounds;
      }

      ZLay_CocoaDrawRect(command);
    } else if (command->type == ZLAY_CMD_TEXT) {
      ZLay_CocoaDrawText(demo, command);
    }
  }
}

static ZLay_RenderCommandList ZLay_CocoaBuildButtonLayout(ZLay_CocoaButtonDemo* demo) {
  ZLay_Theme theme = ZLay_ThemeDark();
  ZLay_Style root_style = ZLay_StyleDefault();
  ZLay_ButtonProps button = ZLay_ButtonPropsDefault(&theme);
  ZLay_NodeDeclarationBuilder root;

  root_style.width = ZLay_Px((float)demo->width);
  root_style.height = ZLay_Px((float)demo->height);
  root_style.padding_left = 32.0f;
  root_style.padding_top = 32.0f;
  root_style.background = theme.canvas;

  button.size = ZLAY_COMPONENT_SIZE_LG;
  if (demo->hovered) {
    button.state |= ZLAY_COMPONENT_STATE_HOVERED;
  }
  if (demo->pressed) {
    button.state |= ZLAY_COMPONENT_STATE_ACTIVE;
  }

  ZLay_SetLayoutDimensions(demo->ctx, ZLay_Dimensions{(float)demo->width, (float)demo->height});
  ZLay_BeginLayout(demo->ctx);

  root = ZLay_BoxDecl(ZLAY_ID("Root"), root_style);
  ZLAY(demo->ctx, ZLAY_ID("Root"), root) {
    ZLay_Button(demo->ctx, ZLAY_ID("OnlyButton"), ZLAY_STRING_LITERAL("ZLay Button"), &button);
  }

  return ZLay_EndLayout(demo->ctx);
}

static void ZLay_CocoaRedraw(ZLay_CocoaButtonDemo* demo) {
  if (demo == NULL || demo->view == NULL) {
    return;
  }

  [(NSView*)demo->view setNeedsDisplay:YES];
}

static void ZLay_CocoaUpdateHover(ZLay_CocoaButtonDemo* demo, NSPoint point) {
  const bool hovered = demo != NULL && ZLay_CocoaRectContains(demo->button_bounds, point);

  if (demo != NULL && demo->hovered != hovered) {
    demo->hovered = hovered;
    ZLay_CocoaRedraw(demo);
  }
}

bool ZLay_CocoaButtonDemo_Init(ZLay_CocoaButtonDemo* demo) {
  ZLay_Config config;
  size_t memory_size;
  NSRect content_rect;
  NSUInteger style;
  NSWindow* window;
  ZLayCocoaButtonView* view;
  ZLayCocoaWindowDelegate* delegate;
  NSFont* font;

  if (demo == NULL) {
    return false;
  }

  *demo = ZLay_CocoaButtonDemo{};
  demo->width = 480;
  demo->height = 240;
  demo->running = true;

  [NSApplication sharedApplication];
  demo->app = NSApp;
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

  font = [[NSFont systemFontOfSize:14.0] retain];
  demo->font = font;

  config = ZLay_ConfigDefault();
  memory_size = ZLay_MinMemorySize(config);
  demo->memory = calloc(1u, memory_size);
  if (demo->memory == NULL) {
    ZLay_CocoaButtonDemo_Shutdown(demo);
    return false;
  }

  demo->arena = ZLay_CreateArenaWithCapacityAndMemory(memory_size, demo->memory);
  demo->ctx = ZLay_Initialize(demo->arena, config);
  if (demo->ctx == NULL) {
    ZLay_CocoaButtonDemo_Shutdown(demo);
    return false;
  }
  ZLay_SetMeasureTextFunction(demo->ctx, ZLay_CocoaMeasureText, demo);

  content_rect = NSMakeRect(0.0, 0.0, (CGFloat)demo->width, (CGFloat)demo->height);
  style = NSWindowStyleMaskTitled |
          NSWindowStyleMaskClosable |
          NSWindowStyleMaskMiniaturizable |
          NSWindowStyleMaskResizable;

  window = [[NSWindow alloc] initWithContentRect:content_rect
                                       styleMask:style
                                         backing:NSBackingStoreBuffered
                                           defer:NO];
  if (window == nil) {
    ZLay_CocoaButtonDemo_Shutdown(demo);
    return false;
  }

  view = [[ZLayCocoaButtonView alloc] initWithFrame:content_rect demo:demo];
  delegate = [[ZLayCocoaWindowDelegate alloc] initWithDemo:demo];
  if (view == nil || delegate == nil) {
    [view release];
    [delegate release];
    [window release];
    ZLay_CocoaButtonDemo_Shutdown(demo);
    return false;
  }

  [window setReleasedWhenClosed:NO];
  [window setTitle:@"ZLay Cocoa Button Layout"];
  [window setDelegate:delegate];
  [window setContentView:view];
  [window setAcceptsMouseMovedEvents:YES];
  [window center];

  demo->window = window;
  demo->view = view;
  demo->window_delegate = delegate;

  [view release];
  [window makeFirstResponder:(NSView*)demo->view];
  [window makeKeyAndOrderFront:nil];
  [NSApp activateIgnoringOtherApps:YES];
  return true;
}

void ZLay_CocoaButtonDemo_Run(ZLay_CocoaButtonDemo* demo) {
  if (demo == NULL || demo->app == NULL || demo->window == NULL) {
    return;
  }

  while (demo->running) {
    @autoreleasepool {
      NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                          untilDate:[NSDate distantFuture]
                                             inMode:NSDefaultRunLoopMode
                                            dequeue:YES];
      if (event != nil) {
        [NSApp sendEvent:event];
      }
      [NSApp updateWindows];
    }
  }
}

void ZLay_CocoaButtonDemo_Shutdown(ZLay_CocoaButtonDemo* demo) {
  if (demo == NULL) {
    return;
  }

  if (demo->window != NULL) {
    NSWindow* window = (NSWindow*)demo->window;
    [window setDelegate:nil];
    [window orderOut:nil];
    [window release];
  }
  if (demo->window_delegate != NULL) {
    [(NSObject*)demo->window_delegate release];
  }
  if (demo->font != NULL) {
    [(NSFont*)demo->font release];
  }

  free(demo->memory);
  *demo = ZLay_CocoaButtonDemo{};
}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  @autoreleasepool {
    ZLay_CocoaButtonDemo demo;

    if (!ZLay_CocoaButtonDemo_Init(&demo)) {
      return 1;
    }

    ZLay_ShellPrintRule(stdout, "ZLay Cocoa Button", ZLAY_SHELL_TONE_ACCENT);
    ZLay_ShellPrintBadge(stdout, "layout", ZLAY_SHELL_TONE_SUCCESS);
    fputs(" one button rendered from ZLay commands. Esc closes the window.\n", stdout);

    ZLay_CocoaButtonDemo_Run(&demo);
    ZLay_CocoaButtonDemo_Shutdown(&demo);
  }

  return 0;
}
