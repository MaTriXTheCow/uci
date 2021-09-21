#include <Board\board.h>

#include <Logger\logger.h>

Board::Board() {
  Logger::Log("Board created");
}

void Board::Draw(WindowProcess winProc) {
  for(int i = 0; i < BOARD_SQUARE_HEIGHT; i++) {
    for(int j = 0; j < BOARD_SQUARE_HEIGHT; j++) {
      int squareColor = ((i + j) % 2) ? WHITE_SQUARE_COLOR : BLACK_SQUARE_COLOR;

      Logger::Log("Setting pixel: ", i, j);

      winProc.FillPixelsRect(i * 80, j * 80, i * 80 + 79, j*80 + 79, squareColor);
    }
  }
}

void Board::Init() {
  Logger::Log("Initialising board");
}
