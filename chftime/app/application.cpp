// Copyright (c) 2016 dacci.org

#include "app/application.h"

#include <crtdbg.h>

#include "ui/main_dialog.h"

CAppModule _Module;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/,
                       wchar_t* /*command_line*/, int /*show_mode*/) {
  HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, nullptr, 0);

#ifdef _DEBUG
  {
    auto flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    flags |=
        _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(flags);
  }
#endif

  SetSearchPathMode(BASE_SEARCH_PATH_ENABLE_SAFE_SEARCHMODE |
                    BASE_SEARCH_PATH_PERMANENT);
  SetDllDirectory(L"");

  auto result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  if (FAILED(result))
    return __LINE__;

  if (!AtlInitCommonControls(0xFFFF))  // all classes
    return __LINE__;

  result = _Module.Init(nullptr, hInstance);
  if (FAILED(result))
    return __LINE__;

  {
    CMessageLoop message_loop;
    if (!_Module.AddMessageLoop(&message_loop))
      return __LINE__;

    MainDialog dialog;
    if (!dialog.Create(NULL))
      return __LINE__;

    dialog.ShowWindow(SW_SHOWNORMAL);
    dialog.UpdateWindow();

    message_loop.Run();

    _Module.RemoveMessageLoop();
  }

  _Module.Term();
  CoUninitialize();

  return 0;
}
