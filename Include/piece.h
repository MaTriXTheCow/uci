#ifndef PIECE_H
#define PIECE_H

#include <stdint.h>
#include <windowProcess.h>
#include <image.h>
#include <imageReader.h>
#include <pieceName.h>

constexpr auto WHITE_PIECE = 1;
constexpr auto BLACK_PIECE = 0;

constexpr auto PAWN   = 2;
constexpr auto BISHOP = 4;
constexpr auto KNIGHT = 8;
constexpr auto ROOK   = 16;
constexpr auto QUEEN  = 32;
constexpr auto KING   = 64;

class Piece {
private:
  uint8_t descriptor;
  uint8_t rank;
  uint8_t file;

  Image* pieceImg;

public:
  Piece(uint8_t, uint8_t, uint8_t, ImageReader*);

  bool Is(uint8_t);
  PieceName GetTypeAsString();

  void Draw(WindowProcess, int);

  static uint8_t GetPieceTypeFromChar(char c);

  int Rank();
  int File();
};

#endif
