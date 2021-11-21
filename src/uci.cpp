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

  board.Init("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
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

      Bitmap* moves = board.GetMoves(board.PieceAt(file, rank));

      board.SetHighlight(moves);
    }
  } else {
    // Check move legality
    Piece* selectedP = board.SelectedPiece();
    Bitmap* moves = board.GetMoves(selectedP);

    // TODO: Special pawn business

    int selectedRank = selectedP->Rank();
    int selectedFile = selectedP->File();

    unsigned int offset = Util::OffsetFromRF(rank, file);
    unsigned int offsetSelected = Util::OffsetFromRF(selectedRank, selectedFile);

    board.Deselect(); 

    // TODO: Special business for pawns and kings

    if (!moves->Has(offset)) {
      // Was not a legal move
      return;
    }

    // Check for pins and such

    Piece* king = board.GetKing(selectedP->GetTypeAsString().color);
    unsigned int kingOffset = Util::OffsetFromRF(king->Rank(), king->File());

    Bitmap pins = board.KingPins(king);
    // TODO: COMPLICATED EN PASSANT PINS

    if (pins.Has(offsetSelected)) {
      // Check if piece stays pinned (GOOD) or goes out of pin (BAD ILLEGAL MOVE)

      // If the moveOffset is between the king and the pinner/the side of the board
      // then the move is ok

      int8_t Rd = ((king->Rank() - selectedRank == 0) ? 0 : ((king->Rank() - selectedRank < 0) ? 1 : -1));
      int8_t Fd = ((king->File() - selectedFile == 0) ? 0 : ((king->File() - selectedFile < 0) ? 1 : -1));

      /*
      IF R+F = 0 is antidiag
      IF R+F = -2 or 2 is diag
      IF R+F = -1 or 1 is rank/file

      IF R*F = 0 is rank/file
      if R*F = -1 is antidiag
      if R*F = 1 is diag

      index = 0 is antidiag
      index = 1 is diag
      index = 2 is file
      index = 3 is rank
      */

      int index;

      if (Rd * Fd == 0) {
        index = abs(Rd);
      } else {
        index = 2 + ((Rd * Fd + 1) / 2);
      }

      OutputDebugString(std::to_string(index).c_str());

      BitmapCollection throughBms[4] = {FILE_OCCUPANCY[kingOffset], RANK_OCCUPANCY[kingOffset], ANTIDIAGONAL_OCCUPANCY[kingOffset], DIAGONAL_OCCUPANCY[kingOffset]};

      if (!(throughBms[index]["lineEx"].Has(offset))) {
        // Goes out of pin
        return;
      }
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
