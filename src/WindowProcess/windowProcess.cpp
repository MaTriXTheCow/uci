#include <windowProcess.h>

#include <Logger\logger.h>

void WindowProcess::SetGameLoopCallback(GameLoopFunc f) {
  gameLoopCallback = f;
}

void WindowProcess::SetHandlerFunction(HandlerFunctions handlerEnum, HandlerFunc f) {
  if(handlerFunctionByName.contains(handlerEnum)) {
    return Logger::LogError("This handler already has a function");
  }

  handlerFunctionByName.insert({handlerEnum, f});
}

void WindowProcess::tryCallHandler(HandlerFunctions handlerEnum) {
  std::map<HandlerFunctions, HandlerFunc>::iterator it = handlerFunctionByName.find(handlerEnum);

  if(it == handlerFunctionByName.end()) return;

  (it->second)();
}

LRESULT CALLBACK WindowProcess::InternalWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch(uMsg) {
    case WM_LBUTTONUP: {
      tryCallHandler(CLICK);
    } break;

    case WM_DESTROY: {
      windowRunning = false;

      tryCallHandler(DESTROY);
    } break;
  }


  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void WindowProcess::Setup(HINSTANCE hInstance, PWSTR /*pCmdLine*/, int nCmdS) {
  // Register the window class.
  const TCHAR CLASS_NAME[]  = TEXT("UCIMainWindow");
  const TCHAR WINDOW_TITLE[] = TEXT("Ultimate Chess Interface Beta");

  WNDCLASS wc{};

  wc.lpfnWndProc   = &WindowProcess::InternalWindowProc;
  wc.hInstance     = hInstance;
  wc.lpszClassName = CLASS_NAME;

  RegisterClass(&wc);

  // Create the window and save hwnd + nCmdShow
  nCmdShow = nCmdS;
  hwnd = CreateWindowEx(
      0,                              // Optional window styles.
      CLASS_NAME,                     // Window class
      WINDOW_TITLE,                   // Window text
      WS_OVERLAPPED | WS_SYSMENU,     // Window style
      10, 10, 656, 679,               // Size and position
      NULL,                           // Parent window
      NULL,                           // Menu
      hInstance,                      // Instance handle
      NULL                            // Additional application data
  );

  if (hwnd == NULL) {
      Logger::LogError("Could not create window.");
      return;
  }
}

void WindowProcess::Show() {
  ShowWindow(hwnd, nCmdShow);

  MSG msg;

  while(windowRunning) {
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    gameLoopCallback();

    Sleep(FRAME_PAUSE);
  }
}
