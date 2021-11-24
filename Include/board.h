#ifndef BOARD_H
#define BOARD_H

#include <windows.h>
#include <stdint.h>
#include <windowProcess.h>
#include <bitmapCollection.h>
#include <piece.h>
#include <constants.h>
#include <unordered_map>
#include <pieceName.h>

static BitmapCollection PAWN_MOVES[64];
static BitmapCollection PAWN_CAPTURES[64];
static BitmapCollection PAWN_DOUBLE_MOVES[64];

inline Bitmap KNIGHT_MOVES[64];
inline Bitmap KING_MOVES[64];

inline BitmapCollection FILE_OCCUPANCY[64];
inline BitmapCollection RANK_OCCUPANCY[64];
inline BitmapCollection DIAGONAL_OCCUPANCY[64];
inline BitmapCollection ANTIDIAGONAL_OCCUPANCY[64];

inline Bitmap IN_BETWEEN[64][64];

class Board {
private:
  BitmapCollection pieces;

  Bitmap enPassantPawns;
  Bitmap highlighted;

  std::unordered_map<std::string, BitmapCollection> piecesMap;

  Piece* pieceAt[8][8];

  unsigned int turn;

  bool castleMap[4];

  bool hasSelected;
  Piece* selectedPiece;

  Bitmap* cachedLegalMoves[64];
  Bitmap hasLegalMovesCacheFor;

  Piece* blackKing;
  Piece* whiteKing;

  bool kingIsChecked;
  bool isDoubleCheck;
  
  Bitmap checkRayAndPiece;
  Piece* checkerPiece;

  bool hasKingPinCache;
  Bitmap kingPins;

public:
  Board();

  void Init(std::string);

  void Draw(WindowProcess);

  void LoadFromFEN(std::string);

  void SetHighlight(Bitmap*);
  void SetHighlight(int, int);
  void ClearHighlight();

  bool HasSelection();
  void SelectPieceAt(int, int);
  void Deselect();

  Piece* SelectedPiece();

  void SetPieceAt(int, int, uint8_t, ImageReader*);
  bool HasPieceAt(int, int);
  bool HasPieceWithColorAt(int, int, uint8_t);
  bool HasSpecificPieceAt(int, int, PieceName);
  Piece* PieceAt(int, int);

  Bitmap* PiecesBitmap(uint8_t);
  Bitmap* PiecesBitmap();
  Bitmap* PieceBitmap(std::string, std::string);


  bool HasEnPassantSquareAt(unsigned int);

  bool IsInBounds(int, int);
  bool IsInBoundsAndCheckPiece(int, int, bool);

  Bitmap* GetAttacks(Piece*);
  void PawnAttacks(Piece*, Bitmap*);
  void RookAttacks(Piece*, Bitmap*);
  void BishopAttacks(Piece*, Bitmap*);
  void KnightAttacks(Piece*, Bitmap*);
  void QueenAttacks(Piece*, Bitmap*);
  void KingAttacks(Piece*, Bitmap*);

  Bitmap* GetMoves(Piece*);
  void PawnMoves(Piece*, Bitmap*);
  void RookMoves(Piece*, Bitmap*);
  void BishopMoves(Piece*, Bitmap*);
  void KnightMoves(Piece*, Bitmap*);
  void QueenMoves(Piece*, Bitmap*);
  void KingMoves(Piece*, Bitmap*);

  Bitmap* GetAttacksFromColor(uint8_t);

  void MakeMove(Piece*, int, int);

  Bitmap LineAttacks(BitmapCollection*);
  Bitmap LineAttacks(Bitmap, BitmapCollection*);
  Bitmap KingPins(Piece*);

  Piece* GetKing(std::string color);
  Piece* GetCheckingPiece();

  Bitmap XrayRookAttacks(Bitmap, unsigned int);
  Bitmap XrayBishopAttacks(Bitmap, unsigned int);

  void GenerateMoveBitmaps();
  void GenerateInBetweenMap();

  unsigned int Turn();

  bool IsTurn(Piece*);
  bool KingChecked();
};

#endif
