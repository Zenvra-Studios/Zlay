/*
  X11 clipboard ownership currently shares window/display state in
  zlay_x11_window.c. This file is reserved for the dedicated clipboard module
  once the backend state is lifted into a private X11 context.
*/
typedef int ZLay_X11_ClipboardModuleSplitPoint;
