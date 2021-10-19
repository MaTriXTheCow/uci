#include <uci.h>

#include <windows.h>
#include <functional>

#include <logger.h>
#include <board.h>

UCI::UCI(HINSTANCE hInstance, PWSTR pCmdLine, int nCmdShow) {
  winProc.Setup(hInstance, pCmdLine, nCmdShow);

  // Setting up handlers
  SetHandlerFunction(CLICK, ClickHandler);
  SetHandlerFunction(DESTROY, DestroyHandler);

  // Setting up game loop
  SetGameLoopCallback([&]() {
    this -> GameLoop();
  });

  board.Init("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBKR b KQkq - 1 2");
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
  board.Draw(winProc);
}
