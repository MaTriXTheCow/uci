#ifndef BOARD_H
#define BOARD_H

#include <windows.h>
#include <stdint.h>
#include <windowProcess.h>
#include <bitmapCollection.h>
#include <piece.h>
#include <constants.h>
#include <unordered_map>

class Board {
private:
  BitmapCollection pieces;

  std::unordered_map<std::string, BitmapCollection> piecesMap;
  

  Piece* pieceAt[8][8];

public:
  Board();

  void Init(std::string);

  void Draw(WindowProcess);

  void LoadFromFEN(std::string);

  void SetPieceAt(int, int, uint8_t, ImageReader*);
  bool HasPieceAt(int, int);
  bool HasSpecificPieceAt(int, int, PieceName);
  Piece* PieceAt(int, int);
};

#endif
