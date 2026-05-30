#include <backends/platform/winrt/internal/zlay_winrt_internal.hpp>

namespace zlay::winrt_internal {

typedef LONG(WINAPI* RtlGetVersionFn)(PRTL_OSVERSIONINFOW version);

bool is_windows_modern_available() {
  HMODULE ntdll = GetModuleHandleA("ntdll.dll");
  if (ntdll == nullptr) {
    return false;
  }

  RtlGetVersionFn rtl_get_version =
    reinterpret_cast<RtlGetVersionFn>(GetProcAddress(ntdll, "RtlGetVersion"));
  if (rtl_get_version == nullptr) {
    return false;
  }

  RTL_OSVERSIONINFOW version = {};
  version.dwOSVersionInfoSize = sizeof(version);
  if (rtl_get_version(&version) != 0) {
    return false;
  }

  return version.dwMajorVersion >= 10;
}

void initialize_apartment() {
#if ZLAY_WINRT_HAS_CPPWINRT
  try {
    winrt::init_apartment(winrt::apartment_type::single_threaded);
  } catch (...) {
  }
#endif
}

void uninitialize_apartment() {
#if ZLAY_WINRT_HAS_CPPWINRT
  try {
    winrt::uninit_apartment();
  } catch (...) {
  }
#endif
}

FARPROC load_proc(const char* dll_name, const char* proc_name) {
  HMODULE module = GetModuleHandleA(dll_name);
  if (module == nullptr) {
    module = LoadLibraryA(dll_name);
  }
  if (module == nullptr) {
    return nullptr;
  }
  return GetProcAddress(module, proc_name);
}

} // namespace zlay::winrt_internal
