#ifndef UCI_H
#define UCI_H

#include <windows.h>

#include <Board\board.h>
#include <WindowProcess\windowProcess.h>

class UCI {
private:
  Board board;
  WindowProcess winProc;

public:
  UCI(HINSTANCE, PWSTR, int);

  int Run();

  void SetHandlerFunction(HandlerFunctions, HandlerFunc);
  void SetGameLoopCallback(std::function<void()> f);

  static void ClickHandler(void);
  static void DestroyHandler(void);

  void GameLoop();
};

#endif
