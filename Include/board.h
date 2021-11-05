#ifndef BOARD_H
#define BOARD_H

#include <windows.h>
#include <stdint.h>
#include <windowProcess.h>
#include <bitmapCollection.h>
#include <piece.h>
#include <constants.h>
#include <unordered_map>

static BitmapCollection PAWN_MOVES[64];
static BitmapCollection PAWN_CAPTURES[64];
static BitmapCollection PAWN_DOUBLE_MOVES[64];

class Board {
private:
  BitmapCollection pieces;

  Bitmap enPassantPawns;
  Bitmap highlighted;

  std::unordered_map<std::string, BitmapCollection> piecesMap;

  Piece* pieceAt[8][8];

  unsigned int turn;

public:
  Board();

  void Init(std::string);

  void Draw(WindowProcess);

  void LoadFromFEN(std::string);

  void SetHighlight(Bitmap*);
  void SetHighlight(int, int);
  void ClearHighlight();

  void SetPieceAt(int, int, uint8_t, ImageReader*);
  bool HasPieceAt(int, int);
  bool HasPieceWithColorAt(int, int, uint8_t);
  bool HasSpecificPieceAt(int, int, PieceName);
  Piece* PieceAt(int, int);

  bool IsInBounds(int, int);
  bool IsInBoundsAndCheckPiece(int, int, bool);

  Bitmap* GetLegalMoves(Piece*);
  void GetPawnMoves(Piece*, Bitmap*);
  void GetRookMoves(Piece*, Bitmap*);
  void GetBishopMoves(Piece*, Bitmap*);
  void GetKnightMoves(Piece*, Bitmap*);
  void GetQueenMoves(Piece*, Bitmap*);
  void GetKingMoves(Piece*, Bitmap*);

  void GenerateMoveBitmaps();
};

#endif
