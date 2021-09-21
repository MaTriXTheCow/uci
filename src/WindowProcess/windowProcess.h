#ifndef WINDOWPROCESS_H
#define WINDOWPROCESS_H

#include <windows.h>
#include <map>
#include <functional>

#define FRAME_PAUSE 100

#define WIDTH 640
#define HEIGHT 640

enum HandlerFunctions{DESTROY, CLICK};

typedef void (*HandlerFunc)();
typedef void (*GameLoopFunc)();

class WindowProcess {
private:
  static inline bool windowRunning = true;

  HWND hwnd;      // For creating HDC
  int nCmdShow;   // later

  HDC hdcPaint;

  DWORD* pBits = NULL; // Pixel array for window

  static inline std::map<HandlerFunctions, HandlerFunc> handlerFunctionByName;
  static void tryCallHandler(HandlerFunctions);

  std::function<void()> gameLoopCallback;

public:
  void Setup(HINSTANCE, PWSTR, int);
  void SetUpdateHandler();
  void Show();

  void SetHandlerFunction(HandlerFunctions, HandlerFunc);
  void SetGameLoopCallback(std::function<void()>);

  static LRESULT CALLBACK InternalWindowProc(HWND, UINT, WPARAM, LPARAM);

  void SetPixelColor(int, int, int);
  void FillPixelsRect(int, int, int, int, int);
};

#endif
