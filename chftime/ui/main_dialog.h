// Copyright (c) 2016 dacci.org

#ifndef CHFTIME_UI_MAIN_DIALOG_H_
#define CHFTIME_UI_MAIN_DIALOG_H_

#include <atlbase.h>
#include <atlwin.h>

#include <atlapp.h>
#include <atlcrack.h>
#include <atlctrls.h>
#include <atlddx.h>

#include <shellapi.h>

#include "res/resource.h"

class MainDialog : public CDialogImpl<MainDialog>,
                   public CWinDataExchange<MainDialog>,
                   private CMessageFilter {
 public:
  static const UINT IDD = IDD_MAIN;

  MainDialog();

  BEGIN_MSG_MAP(MainDialog)
    MSG_WM_INITDIALOG(OnInitDialog)
    MSG_WM_DESTROY(OnDestroy)
    MSG_WM_DROPFILES(OnDropFiles)

    COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
  END_MSG_MAP()

  BEGIN_DDX_MAP(MainDialog)
    DDX_CHECK(IDC_CHANGE_DATE, change_date_)
    DDX_CONTROL_HANDLE(IDC_DATE, date_picker_)
    DDX_CHECK(IDC_CHANGE_TIME, change_time_)
    DDX_CONTROL_HANDLE(IDC_TIME, time_picker_)
    DDX_CHECK(IDC_CHANGE_CREATE, change_create_)
    DDX_CHECK(IDC_CHANGE_WRITE, change_write_)
    DDX_CHECK(IDC_CHANGE_ACCESS, change_access_)
  END_DDX_MAP()

 private:
  bool UpdateTimestamp(FILETIME* file_time, const SYSTEMTIME& date,
                       const SYSTEMTIME& time) const;

  BOOL PreTranslateMessage(MSG* message) override;

  BOOL OnInitDialog(CWindow focus, LPARAM init_param);
  void OnDestroy();
  void OnDropFiles(HDROP drop_info);

  void OnCancel(UINT notify_code, int id, CWindow control);

  bool change_date_;
  CDateTimePickerCtrl date_picker_;
  bool change_time_;
  CDateTimePickerCtrl time_picker_;
  bool change_create_;
  bool change_write_;
  bool change_access_;

  MainDialog(const MainDialog&) = delete;
  MainDialog& operator=(const MainDialog&) = delete;
};

#endif  // CHFTIME_UI_MAIN_DIALOG_H_
