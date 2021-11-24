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

    // TODO: Special business for kings (mostly checking and check-mating)

    Piece* king = board.GetKing(selectedP->GetTypeAsString().color);
    unsigned int kingOffset = Util::OffsetFromRF(king->Rank(), king->File());

    /*if (board.KingChecked()) {
      // Uh-oh king is checked!
      // What to do... what to dooooooOoooOOOOOooOooooOOoOOOooOOOooOoo
      // Either: king moves out, piece captures the checking piece, piece stands in the way of checking piece (king, rook, queen)

      // TODO: Flatten if statements (less branching)

      if (!selectedP->Is(KING)) {
        // King checks for checks already
        Piece* checker = board.GetCheckingPiece();
        unsigned int checkerOffset = Util::OffsetFromRF(checker->Rank(), checker->File());

        if (checker->Is(KNIGHT) || checker->Is(PAWN)) {
          if (offset != checkerOffset) {
            // This move won't work

            return;
          }
        } else {
          if (offset != checkerOffset || !IN_BETWEEN[kingOffset][checkerOffset].Has(offset)) {
            // This move won't work

            return;
          }
        }
      } else {
        // King cannot move in the same line as a pin
        Piece* checker = board.GetCheckingPiece();
        unsigned int checkerOffset = Util::OffsetFromRF(checker->Rank(), checker->File());

        if (checker->Is(ROOK)) {
          if (FILE_OCCUPANCY[checkerOffset]["lineEx"].Or(RANK_OCCUPANCY[checkerOffset]["lineEx"]).Has(offset)) {
            return;
          }
        } else if (checker->Is(BISHOP)) {
          if (DIAGONAL_OCCUPANCY[checkerOffset]["lineEx"].Or(ANTIDIAGONAL_OCCUPANCY[checkerOffset]["lineEx"]).Has(offset)) {
            return;
          }
        } else if (checker->Is(QUEEN)) {
          if (FILE_OCCUPANCY[checkerOffset]["lineEx"].Or(RANK_OCCUPANCY[checkerOffset]["lineEx"]).Or(DIAGONAL_OCCUPANCY[checkerOffset]["lineEx"]).Or(ANTIDIAGONAL_OCCUPANCY[checkerOffset]["lineEx"]).Has(offset)) {
            return;
          }
        }
      }
    }*/

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
