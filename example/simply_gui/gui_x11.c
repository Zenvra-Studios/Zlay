#include <simply_gui/gui_x11.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xatom.h>
#include <X11/keysym.h>

#include <zlay_ui.h>
#include <zlay_shell.h>

/*
  Docs: minimal X11 button example

  This example intentionally renders only one button. The goal is to prove the
  layout path end-to-end before adding a richer widget tree:

  1. X11 owns the window, input events, and simple 2D drawing through Xlib.
  2. ZLay owns the UI tree and layout. Every frame calls ZLay_BeginLayout(),
     declares a root container plus one ZLay_Button(), then calls
     ZLay_EndLayout() to receive render commands.
  3. The X11 renderer below consumes ZLay_RenderCommandList directly. Rect
     commands become XFillRectangle calls; text commands become XDrawString.
  4. Hover/press state is fed back into ZLay_ButtonProps before layout, so the
     button color comes from the component system rather than hand-coded X11
     branches.

  The important bit: do not position the button manually in X11. X11 only draws
  the command rectangles emitted by ZLay. If the button moves, resizes, or
  changes text, that should happen through ZLay styles/components first.

  Controls:
  - Move mouse over the button to see hover styling.
  - Press/release left mouse button to see active styling.
  - Press Escape or close the window to quit.
*/

static unsigned long ZLay_X11Pixel(ZLay_X11ButtonDemo* demo, ZLay_Color color) {
  XColor xcolor;

  if (demo == NULL || demo->display == NULL) {
    return 0;
  }

  xcolor.red = (uint16_t)((uint16_t)color.r * 257u);
  xcolor.green = (uint16_t)((uint16_t)color.g * 257u);
  xcolor.blue = (uint16_t)((uint16_t)color.b * 257u);
  xcolor.flags = DoRed | DoGreen | DoBlue;

  if (XAllocColor(demo->display, DefaultColormap(demo->display, demo->screen), &xcolor) != 0) {
    return xcolor.pixel;
  }

  return (uint32_t)color.r + (uint32_t)color.g + (uint32_t)color.b > 384u
    ? WhitePixel(demo->display, demo->screen)
    : BlackPixel(demo->display, demo->screen);
}

static bool ZLay_X11RectContains(ZLay_Rect rect, int x, int y) {
  return (float)x >= rect.x &&
         (float)y >= rect.y &&
         (float)x < rect.x + rect.width &&
         (float)y < rect.y + rect.height;
}

static ZLay_Dimensions ZLay_X11MeasureText(ZLay_String text, void* user) {
  ZLay_X11ButtonDemo* demo = (ZLay_X11ButtonDemo*)user;
  const int length = text.length > (uint32_t)INT32_MAX ? INT32_MAX : (int)text.length;

  if (demo != NULL && demo->font != NULL && text.chars != NULL) {
    return (ZLay_Dimensions){
      (float)XTextWidth(demo->font, text.chars, length),
      (float)(demo->font->ascent + demo->font->descent)
    };
  }

  return (ZLay_Dimensions){8.0f * (float)text.length, 16.0f};
}

static void ZLay_X11DrawRect(ZLay_X11ButtonDemo* demo, const ZLay_RenderCommand* command) {
  if (command->color.a == 0u) {
    return;
  }

  XSetForeground(demo->display, demo->gc, ZLay_X11Pixel(demo, command->color));
  XFillRectangle(
    demo->display,
    demo->window,
    demo->gc,
    (int)command->bounds.x,
    (int)command->bounds.y,
    (unsigned int)(command->bounds.width < 0.0f ? 0.0f : command->bounds.width),
    (unsigned int)(command->bounds.height < 0.0f ? 0.0f : command->bounds.height)
  );
}

static void ZLay_X11DrawText(ZLay_X11ButtonDemo* demo, const ZLay_RenderCommand* command) {
  const int length = command->text.length > (uint32_t)INT32_MAX ? INT32_MAX : (int)command->text.length;
  int baseline;

  if (command->text.chars == NULL || command->text.length == 0u) {
    return;
  }

  XSetForeground(demo->display, demo->gc, ZLay_X11Pixel(demo, command->color));

  if (demo->font != NULL) {
    baseline = (int)(command->bounds.y +
      ((command->bounds.height - (float)(demo->font->ascent + demo->font->descent)) * 0.5f) +
      (float)demo->font->ascent);
  } else {
    baseline = (int)(command->bounds.y + command->bounds.height);
  }

  XDrawString(
    demo->display,
    demo->window,
    demo->gc,
    (int)command->bounds.x,
    baseline,
    command->text.chars,
    length
  );
}

static void ZLay_X11RenderCommands(ZLay_X11ButtonDemo* demo, ZLay_RenderCommandList commands) {
  demo->button_bounds = (ZLay_Rect){0};

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

      ZLay_X11DrawRect(demo, command);
    } else if (command->type == ZLAY_CMD_TEXT) {
      ZLay_X11DrawText(demo, command);
    }
  }

  XFlush(demo->display);
}

static ZLay_RenderCommandList ZLay_X11BuildButtonLayout(ZLay_X11ButtonDemo* demo) {
  ZLay_Theme theme = ZLay_ThemeDark();
  ZLay_Style root_style = ZLay_StyleDefault();
  ZLay_ButtonProps button = ZLay_ButtonPropsDefault(&theme);

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

  ZLay_SetLayoutDimensions(demo->ctx, (ZLay_Dimensions){(float)demo->width, (float)demo->height});
  ZLay_BeginLayout(demo->ctx);

  ZLAY(demo->ctx, ZLAY_ID("Root"), ZLAY_BOX(.id = ZLAY_ID("Root"), .style = root_style)) {
    ZLay_Button(demo->ctx, ZLAY_ID("OnlyButton"), ZLAY_STRING_LITERAL("ZLay Button"), &button);
  }

  return ZLay_EndLayout(demo->ctx);
}

static void ZLay_X11Redraw(ZLay_X11ButtonDemo* demo) {
  ZLay_RenderCommandList commands = ZLay_X11BuildButtonLayout(demo);
  ZLay_X11RenderCommands(demo, commands);
}

static void ZLay_X11UpdateHover(ZLay_X11ButtonDemo* demo, int x, int y) {
  const bool hovered = ZLay_X11RectContains(demo->button_bounds, x, y);

  if (demo->hovered != hovered) {
    demo->hovered = hovered;
    ZLay_X11Redraw(demo);
  }
}

bool ZLay_X11ButtonDemo_Init(ZLay_X11ButtonDemo* demo) {
  ZLay_Config config;
  size_t memory_size;
  XSetWindowAttributes attributes;

  if (demo == NULL) {
    return false;
  }

  *demo = (ZLay_X11ButtonDemo){0};
  demo->width = 480;
  demo->height = 240;
  demo->running = true;

  demo->display = XOpenDisplay(NULL);
  if (demo->display == NULL) {
    ZLay_ShellPrintfStyled(
      stderr,
      ZLay_ShellStyleStrong(ZLAY_SHELL_TONE_ERROR),
      "ZLay X11: unable to open display. Is DISPLAY set?\n"
    );
    return false;
  }

  demo->screen = DefaultScreen(demo->display);
  attributes.background_pixel = ZLay_X11Pixel(demo, (ZLay_Color){11, 15, 24, 255});
  attributes.event_mask = ExposureMask |
                          StructureNotifyMask |
                          PointerMotionMask |
                          LeaveWindowMask |
                          ButtonPressMask |
                          ButtonReleaseMask |
                          KeyPressMask;

  demo->window = XCreateWindow(
    demo->display,
    RootWindow(demo->display, demo->screen),
    0,
    0,
    (unsigned int)demo->width,
    (unsigned int)demo->height,
    0,
    CopyFromParent,
    InputOutput,
    CopyFromParent,
    CWBackPixel | CWEventMask,
    &attributes
  );

  XStoreName(demo->display, demo->window, "ZLay X11 Button Layout");
  demo->wm_delete_window = XInternAtom(demo->display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(demo->display, demo->window, &demo->wm_delete_window, 1);

  demo->gc = XCreateGC(demo->display, demo->window, 0, NULL);
  demo->font = XLoadQueryFont(demo->display, "fixed");
  if (demo->font != NULL) {
    XSetFont(demo->display, demo->gc, demo->font->fid);
  }

  config = ZLay_ConfigDefault();
  memory_size = ZLay_MinMemorySize(config);
  demo->memory = calloc(1u, memory_size);
  if (demo->memory == NULL) {
    ZLay_X11ButtonDemo_Shutdown(demo);
    return false;
  }

  demo->arena = ZLay_CreateArenaWithCapacityAndMemory(memory_size, demo->memory);
  demo->ctx = ZLay_Initialize(demo->arena, config);
  if (demo->ctx == NULL) {
    ZLay_X11ButtonDemo_Shutdown(demo);
    return false;
  }

  ZLay_SetMeasureTextFunction(demo->ctx, ZLay_X11MeasureText, demo);
  XMapWindow(demo->display, demo->window);
  return true;
}

void ZLay_X11ButtonDemo_Run(ZLay_X11ButtonDemo* demo) {
  if (demo == NULL || demo->display == NULL) {
    return;
  }

  while (demo->running) {
    XEvent event;
    XNextEvent(demo->display, &event);

    switch (event.type) {
      case Expose:
        if (event.xexpose.count == 0) {
          ZLay_X11Redraw(demo);
        }
        break;
      case ConfigureNotify:
        demo->width = event.xconfigure.width;
        demo->height = event.xconfigure.height;
        ZLay_X11Redraw(demo);
        break;
      case MotionNotify:
        ZLay_X11UpdateHover(demo, event.xmotion.x, event.xmotion.y);
        break;
      case LeaveNotify:
        if (demo->hovered || demo->pressed) {
          demo->hovered = false;
          demo->pressed = false;
          ZLay_X11Redraw(demo);
        }
        break;
      case ButtonPress:
        if (event.xbutton.button == Button1 &&
            ZLay_X11RectContains(demo->button_bounds, event.xbutton.x, event.xbutton.y)) {
          demo->pressed = true;
          demo->hovered = true;
          ZLay_X11Redraw(demo);
        }
        break;
      case ButtonRelease:
        if (event.xbutton.button == Button1) {
          demo->pressed = false;
          demo->hovered = ZLay_X11RectContains(demo->button_bounds, event.xbutton.x, event.xbutton.y);
          ZLay_X11Redraw(demo);
        }
        break;
      case KeyPress:
        if (XLookupKeysym(&event.xkey, 0) == XK_Escape) {
          demo->running = false;
        }
        break;
      case ClientMessage:
        if ((Atom)event.xclient.data.l[0] == demo->wm_delete_window) {
          demo->running = false;
        }
        break;
      default:
        break;
    }
  }
}

void ZLay_X11ButtonDemo_Shutdown(ZLay_X11ButtonDemo* demo) {
  if (demo == NULL) {
    return;
  }

  if (demo->display != NULL) {
    if (demo->font != NULL) {
      XFreeFont(demo->display, demo->font);
    }
    if (demo->gc != 0) {
      XFreeGC(demo->display, demo->gc);
    }
    if (demo->window != 0) {
      XDestroyWindow(demo->display, demo->window);
    }
    XCloseDisplay(demo->display);
  }

  free(demo->memory);
  *demo = (ZLay_X11ButtonDemo){0};
}

int main(void) {
  ZLay_X11ButtonDemo demo;

  if (!ZLay_X11ButtonDemo_Init(&demo)) {
    return 1;
  }

  ZLay_ShellPrintRule(stdout, "ZLay X11 Button", ZLAY_SHELL_TONE_ACCENT);
  ZLay_ShellPrintBadge(stdout, "layout", ZLAY_SHELL_TONE_SUCCESS);
  fputs(" one button rendered from ZLay commands. Esc closes the window.\n", stdout);

  ZLay_X11ButtonDemo_Run(&demo);
  ZLay_X11ButtonDemo_Shutdown(&demo);
  return 0;
}
