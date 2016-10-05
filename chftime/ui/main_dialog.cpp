// Copyright (c) 2016 dacci.org

#include "ui/main_dialog.h"

#include <atlstr.h>

#include "app/application.h"

namespace {

const FILETIME kUnchange{MAXDWORD, MAXDWORD};

class File {
 public:
  explicit File(LPCTSTR path) {
    handle_ =
        CreateFile(path, FILE_WRITE_ATTRIBUTES,
                   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                   nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (IsValid())
      SetFileTime(nullptr, &kUnchange, &kUnchange);
  }

  ~File() {
    if (IsValid()) {
      CloseHandle(handle_);
      handle_ = INVALID_HANDLE_VALUE;
    }
  }

  bool GetFileTime(FILETIME* creation_time, FILETIME* last_access_time,
                   FILETIME* last_write_time) const {
    return ::GetFileTime(handle_, creation_time, last_access_time,
                         last_write_time) != FALSE;
  }

  bool SetFileTime(const FILETIME* creation_time,
                   const FILETIME* last_access_time,
                   const FILETIME* last_write_time) {
    return ::SetFileTime(handle_, creation_time, last_access_time,
                         last_write_time) != FALSE;
  }

  bool IsValid() const {
    return handle_ != INVALID_HANDLE_VALUE;
  }

 private:
  HANDLE handle_;
};

}  // namespace

MainDialog::MainDialog()
    : change_date_(true),
      change_time_(true),
      change_create_(false),
      change_write_(true),
      change_access_(false) {}

bool MainDialog::UpdateTimestamp(FILETIME* file_time, const SYSTEMTIME& date,
                                 const SYSTEMTIME& time) const {
  SYSTEMTIME system_time;
  if (!FileTimeToSystemTime(file_time, &system_time))
    return false;

  if (change_date_) {
    system_time.wYear = date.wYear;
    system_time.wMonth = date.wMonth;
    system_time.wDayOfWeek = date.wDayOfWeek;
    system_time.wDay = date.wDay;
  }

  if (change_time_) {
    system_time.wHour = time.wHour;
    system_time.wMinute = time.wMinute;
    system_time.wSecond = time.wSecond;
    system_time.wMilliseconds = 0;
  }

  if (!SystemTimeToFileTime(&system_time, file_time))
    return false;

  if (file_time->dwHighDateTime == 0 && file_time->dwLowDateTime == 0)
    file_time->dwLowDateTime = 1;

  return true;
}

BOOL MainDialog::PreTranslateMessage(MSG* message) {
  return IsDialogMessage(message);
}

BOOL MainDialog::OnInitDialog(CWindow /*focus*/, LPARAM /*init_param*/) {
  SetIcon(AtlLoadIcon(IDD), FALSE);

  DoDataExchange(DDX_LOAD);

  _Module.GetMessageLoop()->AddMessageFilter(this);

  return TRUE;
}

void MainDialog::OnDestroy() {
  _Module.GetMessageLoop()->RemoveMessageFilter(this);

  PostQuitMessage(0);
}

void MainDialog::OnDropFiles(HDROP drop_info) {
  DoDataExchange(DDX_SAVE);

  SYSTEMTIME date, time;
  date_picker_.GetSystemTime(&date);
  time_picker_.GetSystemTime(&time);

  auto count = DragQueryFile(drop_info, UINT_MAX, nullptr, 0);

  for (auto i = 0U; i < count; ++i) {
    TCHAR path[MAX_PATH];
    auto length = DragQueryFile(drop_info, i, path, _countof(path));
    if (length == 0)
      continue;

    File file(path);
    if (!file.IsValid())
      continue;

    FILETIME creation_time, access_time, write_time;
    if (!file.GetFileTime(&creation_time, &access_time, &write_time))
      continue;

    if (!UpdateTimestamp(&creation_time, date, time))
      continue;

    if (!UpdateTimestamp(&access_time, date, time))
      continue;

    if (!UpdateTimestamp(&write_time, date, time))
      continue;

    auto new_creation_time = change_create_ ? &creation_time : nullptr;
    auto new_access_time = change_access_ ? &access_time : nullptr;
    auto new_write_time = change_write_ ? &write_time : nullptr;
    file.SetFileTime(new_creation_time, new_access_time, new_write_time);
  }

  DragFinish(drop_info);
}

void MainDialog::OnCancel(UINT /*notify_code*/, int /*id*/,
                          CWindow /*control*/) {
  DestroyWindow();
}
