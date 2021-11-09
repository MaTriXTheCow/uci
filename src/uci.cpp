#include <uci.h>

#include <windows.h>
#include <functional>

#include <iostream>
#include <string>

#include <logger.h>
#include <board.h>
#include <constants.h>
#include <util.h>

UCI::UCI(HINSTANCE hInstance, PWSTR pCmdLine, int nCmdShow) {
  winProc.Setup(hInstance, pCmdLine, nCmdShow);

  // Setting up handlers
  SETHANDLERFUNCTION(CLICK, ClickHandler);
  SETHANDLERFUNCTION(DESTROY, DestroyHandler);

  // Setting up game loop
  SetGameLoopCallback([&]() {
    this -> GameLoop();
  });

  board.Init("rnbqkbnr/pppppppp/8/8/8/8/PP1PPPPP/RNBQKBNR w KQkq - 0 1");
}

int UCI::Run() {
  winProc.Show();

  return 0;
}

void UCI::ClickHandler() {
  int file = WindowProcess::GetXClick()/SQUARE_MEASUREMENT + 1;
  int rank = (HEIGHT - WindowProcess::GetYClick())/SQUARE_MEASUREMENT + 1;

  board.ClearHighlight();

  if (!board.HasSelection()) {
    if (board.HasPieceAt(rank, file) && board.IsTurn(board.PieceAt(file, rank))) {
      board.SelectPieceAt(rank, file);

      Bitmap* moves = board.GetLegalMoves(board.PieceAt(file, rank));

      board.SetHighlight(moves);
    }
  } else {
    // Check move legality
    Piece* selectedP = board.SelectedPiece();

    Bitmap* moves = board.GetLegalMoves(selectedP);

    unsigned int offset = Util::OffsetFromRF(rank, file);

    board.Deselect();

    if (!moves->Has(offset)) {
      // Was not a legal move
      return;
    }

    board.MakeMove(selectedP, rank, file);
  }
}

void UCI::DestroyHandler() {
  Logger::Log("Destroying window.");
}

void UCI::GameLoop() {
  board.Draw(winProc);
}
