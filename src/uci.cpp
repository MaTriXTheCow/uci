#include <uci.h>

#include <windows.h>
#include <functional>

#include <iostream>
#include <string>

#include <logger.h>
#include <board.h>
#include <constants.h>

UCI::UCI(HINSTANCE hInstance, PWSTR pCmdLine, int nCmdShow) {
  winProc.Setup(hInstance, pCmdLine, nCmdShow);

  // Setting up handlers
  SETHANDLERFUNCTION(CLICK, ClickHandler);
  SETHANDLERFUNCTION(DESTROY, DestroyHandler);

  // Setting up game loop
  SetGameLoopCallback([&]() {
    this -> GameLoop();
  });

  board.Init("rnbqkbnr/pppppppp/8/8/3pP3/8/PPPPPPPP/RBNQKNBR b KQkq - 1 2");
}

int UCI::Run() {
  winProc.Show();

  return 0;
}

void UCI::ClickHandler() {
  int file = WindowProcess::GetXClick()/SQUARE_MEASUREMENT + 1;
  int rank = (HEIGHT - WindowProcess::GetYClick())/SQUARE_MEASUREMENT + 1;

  board.ClearHighlight();

  if (board.HasPieceAt(rank, file)) {
    board.SetHighlight(rank, file);

    Bitmap* moves = board.GetLegalMoves(board.PieceAt(file, rank));

    board.SetHighlight(moves);
  }
}

void UCI::DestroyHandler() {
  Logger::Log("Destroying window.");
}

void UCI::GameLoop() {
  board.Draw(winProc);
}
