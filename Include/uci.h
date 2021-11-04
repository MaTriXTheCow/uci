#ifndef UCI_H
#define UCI_H

#include <windows.h>

#include <board.h>
#include <windowProcess.h>

// Absolutely SICK macro

#define SETHANDLERFUNCTION(enm,fnc) SetHandlerFunction(enm,[=,this](){this->fnc();})

class UCI {
private:
  Board board;
  WindowProcess winProc;

public:
  UCI(HINSTANCE, PWSTR, int);

  int Run();

  void SetGameLoopCallback(std::function<void()> f);

  void SetHandlerFunction(HandlerFunctions handlerEnum, std::function<void()> f);

  void ClickHandler(void);
  static void DestroyHandler(void);

  void GameLoop();
};

#endif
