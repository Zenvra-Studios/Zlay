#ifndef ZLAY_BACKENDS_PLATFORM_WINRT_INTERNAL_ZLAY_WINRT_INTERNAL_HPP
#define ZLAY_BACKENDS_PLATFORM_WINRT_INTERNAL_ZLAY_WINRT_INTERNAL_HPP

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winternl.h>

#if defined(__has_include)
#  if __has_include(<winrt/base.h>)
#    define ZLAY_WINRT_HAS_CPPWINRT 1
#    include <winrt/base.h>
#    include <winrt/Windows.Foundation.h>
#    include <winrt/Windows.UI.ViewManagement.h>
#    include <winrt/Windows.UI.Composition.h>
#    include <winrt/Windows.Storage.h>
#    include <winrt/Windows.Storage.Pickers.h>
#    include <winrt/Windows.UI.Notifications.h>
#  else
#    define ZLAY_WINRT_HAS_CPPWINRT 0
#  endif
#else
#  define ZLAY_WINRT_HAS_CPPWINRT 0
#endif

namespace zlay::winrt_internal {

bool is_windows_modern_available();
void initialize_apartment();
void uninitialize_apartment();
FARPROC load_proc(const char* dll_name, const char* proc_name);

} // namespace zlay::winrt_internal

#endif
