#pragma once

#if defined(_WIN32)
#  include "Window32.h"
#elif defined(__APPLE__)
#  include "WindowCocoa.h"
#elif defined(__linux__)
#  include "WindowX11.h"
#else
#  error "ZLay example platform window is not implemented for this platform"
#endif
