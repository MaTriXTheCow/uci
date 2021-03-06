#ifndef WINDOWPROCESS_H
#define WINDOWPROCESS_H

#include <windows.h>
#include <map>
#include <functional>
#include <image.h>

constexpr auto FRAME_PAUSE = 100;

constexpr auto WIDTH = 640;
constexpr auto HEIGHT = 640;

constexpr auto SQUARE_MEASUREMENT = HEIGHT/8;

enum HandlerFunctions{DESTROY, CLICK};

typedef std::function<void()> HandlerFunc;
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

  static int lastXClick;
  static int lastYClick;

public:
  void Setup(HINSTANCE, PWSTR, int);;
  void Show();

  void SetHandlerFunction(HandlerFunctions, HandlerFunc);
  void SetGameLoopCallback(std::function<void()>);

  static LRESULT CALLBACK InternalWindowProc(HWND, UINT, WPARAM, LPARAM);

  void SetPixelColor(int, int, int);
  void FillPixelsSquare(int, int, int, int);
  void DrawImageSquare(int, int, Image*, int);

  static int GetXClick();
  static int GetYClick();
};

#endif
