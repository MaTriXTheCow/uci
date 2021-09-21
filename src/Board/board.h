#ifndef BOARD_H
#define BOARD_H

#include <windows.h>
#include <..\WindowProcess\windowProcess.h>

#define BOARD_SQUARE_WIDTH 8
#define BOARD_SQUARE_HEIGHT 8

#define WHITE_SQUARE_COLOR 0x884422
#define BLACK_SQUARE_COLOR 0x44BB11

class Board {
private:


public:
  Board();

  void Init();

  void Draw(WindowProcess);
};

#endif
