#include <Board\board.h>

#include <Logger\logger.h>

Board::Board() : pieceAt(0xFFFF00000000FFFFULL), pawnAt(0x00FF00000000FF00ULL) {
  Logger::Log("Board created");
}

void Board::Draw(WindowProcess winProc) {
  for(int rank = 1; rank <= BOARD_SQUARE_HEIGHT; rank++) {
    for(int file = 1; file <= BOARD_SQUARE_WIDTH; file++) {
      int squareColor = ((rank + file) % 2) ? WHITE_SQUARE_COLOR : BLACK_SQUARE_COLOR;

      if(HasPieceAt(rank, file)) {
        winProc.FillPixelsSquare(rank, file, SQUARE_MEASUREMENT, 0xFF55FF);
      } else {
        winProc.FillPixelsSquare(rank, file, SQUARE_MEASUREMENT, squareColor);
      }
    }
  }
}

void Board::Init() {
  Logger::Log("Initialising board");
}

bool Board::HasPieceAt(int rank, int file) {
  unsigned int offset = (rank-1) * BOARD_SQUARE_WIDTH + (file-1);

  return pieceAt.Has(offset);
}
