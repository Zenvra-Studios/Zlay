#include <os/winrt/zlay_winrt_picker.h>

#include <os/winrt/internal/zlay_winrt_internal.hpp>

#include <commdlg.h>
#include <shlobj.h>

typedef BOOL(WINAPI* ZLay_GetOpenFileNameAFn)(LPOPENFILENAMEA data);
typedef BOOL(WINAPI* ZLay_GetSaveFileNameAFn)(LPOPENFILENAMEA data);
typedef PIDLIST_ABSOLUTE(WINAPI* ZLay_SHBrowseForFolderAFn)(LPBROWSEINFOA data);
typedef BOOL(WINAPI* ZLay_SHGetPathFromIDListAFn)(PCIDLIST_ABSOLUTE pidl, LPSTR path);

static void ZLay_WinRT_PickerCopyText(char* dst, size_t dst_capacity, const char* src) {
  if (dst == nullptr || dst_capacity == 0u) {
    return;
  }

  if (src == nullptr) {
    dst[0] = '\0';
    return;
  }

  lstrcpynA(dst, src, static_cast<int>(dst_capacity));
}

static bool ZLay_WinRT_PickerValidate(const ZLay_WinRTPickerDesc* desc) {
  return desc != nullptr && desc->out_path != nullptr && desc->out_path_capacity > 0u;
}

static bool ZLay_WinRT_PickerFileDialog(const ZLay_WinRTPickerDesc* desc, bool save) {
  if (!ZLay_WinRT_PickerValidate(desc)) {
    return false;
  }

  OPENFILENAMEA data = {};
  char path[MAX_PATH] = {};
  ZLay_WinRT_PickerCopyText(path, sizeof(path), desc->out_path);

  data.lStructSize = sizeof(data);
  data.hwndOwner = static_cast<HWND>(desc->owner_window);
  data.lpstrFile = path;
  data.nMaxFile = sizeof(path);
  data.lpstrTitle = desc->title;
  data.lpstrFilter = desc->filter;
  data.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
  if (!save) {
    data.Flags |= OFN_FILEMUSTEXIST;
  } else {
    data.Flags |= OFN_OVERWRITEPROMPT;
  }

  if (save) {
    ZLay_GetSaveFileNameAFn get_save_file_name = reinterpret_cast<ZLay_GetSaveFileNameAFn>(
      zlay::winrt_internal::load_proc("comdlg32.dll", "GetSaveFileNameA"));
    if (get_save_file_name == nullptr || get_save_file_name(&data) == 0) {
      return false;
    }
  } else {
    ZLay_GetOpenFileNameAFn get_open_file_name = reinterpret_cast<ZLay_GetOpenFileNameAFn>(
      zlay::winrt_internal::load_proc("comdlg32.dll", "GetOpenFileNameA"));
    if (get_open_file_name == nullptr || get_open_file_name(&data) == 0) {
      return false;
    }
  }

  ZLay_WinRT_PickerCopyText(desc->out_path, desc->out_path_capacity, path);
  return true;
}

bool ZLay_WinRT_PickerIsAvailable(void) {
  return zlay::winrt_internal::is_windows_modern_available();
}

bool ZLay_WinRT_PickerOpenFile(const ZLay_WinRTPickerDesc* desc) {
  return ZLay_WinRT_PickerFileDialog(desc, false);
}

bool ZLay_WinRT_PickerSaveFile(const ZLay_WinRTPickerDesc* desc) {
  return ZLay_WinRT_PickerFileDialog(desc, true);
}

bool ZLay_WinRT_PickerPickFolder(const ZLay_WinRTPickerDesc* desc) {
  if (!ZLay_WinRT_PickerValidate(desc)) {
    return false;
  }

  ZLay_SHBrowseForFolderAFn browse_for_folder = reinterpret_cast<ZLay_SHBrowseForFolderAFn>(
    zlay::winrt_internal::load_proc("shell32.dll", "SHBrowseForFolderA"));
  ZLay_SHGetPathFromIDListAFn get_path_from_id_list = reinterpret_cast<ZLay_SHGetPathFromIDListAFn>(
    zlay::winrt_internal::load_proc("shell32.dll", "SHGetPathFromIDListA"));
  if (browse_for_folder == nullptr || get_path_from_id_list == nullptr) {
    return false;
  }

  BROWSEINFOA data = {};
  char path[MAX_PATH] = {};
  data.hwndOwner = static_cast<HWND>(desc->owner_window);
  data.lpszTitle = desc->title;
  data.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

  PIDLIST_ABSOLUTE pidl = browse_for_folder(&data);
  if (pidl == nullptr) {
    return false;
  }

  bool ok = get_path_from_id_list(pidl, path) != 0;
  CoTaskMemFree(pidl);
  if (!ok) {
    return false;
  }

  ZLay_WinRT_PickerCopyText(desc->out_path, desc->out_path_capacity, path);
  return true;
}
