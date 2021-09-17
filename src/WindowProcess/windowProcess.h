#ifndef WINDOWPROCESS_H
#define WINDOWPROCESS_H

#include <windows.h>
#include <map>

#define FRAME_PAUSE 100

enum HandlerFunctions{DESTROY, CLICK};

typedef void (*HandlerFunc)();
typedef void (*GameLoopFunc)();

class WindowProcess {
private:
  static inline bool windowRunning = true;

  HWND hwnd;      // For creating HDC
  int nCmdShow;   // later

  static inline std::map<HandlerFunctions, HandlerFunc> handlerFunctionByName;
  static void tryCallHandler(HandlerFunctions);

  GameLoopFunc gameLoopCallback;

public:
  void Setup(HINSTANCE, PWSTR, int);
  void SetUpdateHandler();
  void Show();

  void SetHandlerFunction(HandlerFunctions, HandlerFunc);

  void SetGameLoopCallback(GameLoopFunc);

  static LRESULT CALLBACK InternalWindowProc(HWND, UINT, WPARAM, LPARAM);
};

#endif
