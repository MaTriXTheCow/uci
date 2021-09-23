#ifndef BOARD_H
#define BOARD_H

#include <windows.h>
#include <stdint.h>
#include <..\WindowProcess\windowProcess.h>
#include <..\Util\Bitmap\bitmap.h>

#define BOARD_SQUARE_WIDTH  8
#define BOARD_SQUARE_HEIGHT 8

#define WHITE_SQUARE_COLOR 0xEBECD0
#define BLACK_SQUARE_COLOR 0x779556

class Board {
private:
  Bitmap pieceAt;
  Bitmap pawnAt;

public:
  Board();

  void Init();

  void Draw(WindowProcess);

  bool HasPieceAt(int rank, int file);
};

#endif
