#include <uci.h>

#include <windows.h>

#include <Logger\logger.h>
#include <Board\board.h>

UCI::UCI(HINSTANCE hInstance, PWSTR pCmdLine, int nCmdShow) {
  winProc.Setup(hInstance, pCmdLine, nCmdShow);

  // Setting up handlers
  winProc.SetHandlerFunction(CLICK, ClickHandler);
  winProc.SetHandlerFunction(DESTROY, DestroyHandler);

  // Setting up game loop
  winProc.SetGameLoopCallback(GameLoop);
}

int UCI::Run() {
  winProc.Show();

  return 0;
}

void UCI::ClickHandler() {
  Logger::Log("Clicked.");
}

void UCI::DestroyHandler() {
  Logger::Log("Destroying window.");
}

void UCI::GameLoop() {
  
}
