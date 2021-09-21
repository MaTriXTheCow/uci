#include <windowProcess.h>

#include <Logger\logger.h>

HDC hdcMem = NULL;

void WindowProcess::tryCallHandler(HandlerFunctions handlerEnum) {
  std::map<HandlerFunctions, HandlerFunc>::iterator it = handlerFunctionByName.find(handlerEnum);

  if(it == handlerFunctionByName.end()) return;

  (it->second)();
}

void WindowProcess::SetGameLoopCallback(std::function<void()> f) {
  gameLoopCallback = f;
}


void WindowProcess::SetHandlerFunction(HandlerFunctions handlerEnum, HandlerFunc f) {
  if(handlerFunctionByName.contains(handlerEnum)) {
    return Logger::LogError("This handler already has a function");
  }

  handlerFunctionByName.insert({handlerEnum, f});
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

    case WM_PAINT: {
      PAINTSTRUCT ps;
      BeginPaint(hwnd, &ps);

      BitBlt(ps.hdc, 0, 0, WIDTH, HEIGHT, hdcMem, 0, 0, SRCCOPY);

      EndPaint(hwnd, &ps);
    } break;
  }


  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void WindowProcess::SetPixelColor(int y, int x, int color) {
  int b = (color >> 0)  & 0xff;
  int g = (color >> 8)  & 0xff;
  int r = (color >> 16) & 0xff;

  int inverseY = HEIGHT - y - 1;

  pBits[(inverseY * WIDTH) + x] = RGB(r, g, b);
}

void WindowProcess::FillPixelsRect(int y0, int y1, int x0, int x1, int color) {
  for(int i = y0; i < y1; i++) {
    for(int j = x0; j < x1; j++) {
      SetPixelColor(i,j,color);
    }
  }
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

  BITMAPINFO bmp    = {0};
  HBITMAP    hBmp   = NULL;

  bmp.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
  bmp.bmiHeader.biWidth       = WIDTH;
  bmp.bmiHeader.biHeight      = HEIGHT;
  bmp.bmiHeader.biPlanes      = 1;
  bmp.bmiHeader.biBitCount    = 32;
  bmp.bmiHeader.biCompression = BI_RGB;

  hBmp   = CreateDIBSection(NULL, &bmp, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
  hdcMem = CreateCompatibleDC(NULL);

  SelectObject(hdcMem, hBmp);
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

    InvalidateRect(hwnd, NULL, FALSE);

    Sleep(FRAME_PAUSE);
  }
}
