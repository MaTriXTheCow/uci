#include <board.h>

#include <logger.h>
#include <piece.h>
#include <util.h>
#include <imageReader.h>

#include <array>

Board::Board() {
  Logger::Log("Board created");

  turn = 0;
  hasSelected = false;
  selectedPiece = nullptr;

  

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      pieceAt[i][j] = nullptr;
      hasLegalMovesCacheFor[i * 8 + j] = false;
      cachedLegalMoves[i * 8 + j] = NULL;
    }
  }

  piecesMap["pawns"]   = BitmapCollection();
  piecesMap["rooks"]   = BitmapCollection();
  piecesMap["bishops"] = BitmapCollection();
  piecesMap["knights"] = BitmapCollection();
  piecesMap["queens"]  = BitmapCollection();
  piecesMap["kings"]   = BitmapCollection();
}

void Board::Draw(WindowProcess winProc) {
  for(int rank = 1; rank <= BOARD_SQUARE_HEIGHT; rank++) {
    for(int file = 1; file <= BOARD_SQUARE_WIDTH; file++) {
      int squareColor = ((rank + file) % 2) ? WHITE_SQUARE_COLOR : BLACK_SQUARE_COLOR;

      unsigned int offset = Util::OffsetFromRF(rank, file);

      if(HasPieceAt(rank, file)) {
        if (highlighted.Has(offset)) {
          int highlightColor = (squareColor == WHITE_SQUARE_COLOR) ? 0xf9f97a : 0xb4cd21;

          PieceAt(file, rank)->Draw(winProc, highlightColor);
          continue;
        }

        PieceAt(file, rank)->Draw(winProc, squareColor);
      } else {
        if (highlighted.Has(offset)) {
          int highlightColor = (squareColor == WHITE_SQUARE_COLOR) ? 0xf9f97a : 0xb4cd21;

          winProc.FillPixelsSquare(rank, file, SQUARE_MEASUREMENT, highlightColor);
          continue;
        }

        winProc.FillPixelsSquare(rank, file, SQUARE_MEASUREMENT, squareColor);
      }
    }
  }
}

void Board::Init(std::string fen) {
  Logger::Log("Initialising board");

  LoadFromFEN(fen);

  GenerateMoveBitmaps();
}

void Board::GenerateMoveBitmaps() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      int rank = i + 1;
      int file = j + 1;

      uint64_t offset = Util::OffsetFromRF(rank, file);

      // Occupancy line bitmaps
      FILE_OCCUPANCY[offset]["lineEx"].SetAll((0x8080808080808080 ^ (0x8000000000000000 >> (8 * (8-rank)))) >> (8-file));
      FILE_OCCUPANCY[offset]["lower"] = FILE_OCCUPANCY[offset]["lineEx"].And((1ULL << offset) - 1);
      FILE_OCCUPANCY[offset]["upper"] = FILE_OCCUPANCY[offset]["lineEx"].And(~((1ULL << offset) - 1));

      RANK_OCCUPANCY[offset]["lineEx"].SetAll((0xFF00000000000000 ^ (0x8000000000000000 >> (8 - file))) >> (8*(8 - rank)));
      RANK_OCCUPANCY[offset]["lower"] = RANK_OCCUPANCY[offset]["lineEx"].And((1ULL << (offset+1)) - 1);
      RANK_OCCUPANCY[offset]["upper"] = RANK_OCCUPANCY[offset]["lineEx"].And(~((1ULL << (offset+1)) - 1));

      uint64_t n = 0x8040201008040201;
      int shift = (file-1)-(rank-1);

      if (shift > 0) {
        n <<= shift;
        n &= (0x8000000000000000 >> (8 * (shift-1)))-1;
      } else if(shift < 0) {
        n >>= -shift;
        n &= ~((0x0000000000000001ULL << (8 * (-shift - 1))) - 1);
      }

      n ^= (0x0000000000000001ULL << (8 * (rank - 1) + (file - 1)));

      DIAGONAL_OCCUPANCY[offset]["lineEx"].SetAll(n);
      DIAGONAL_OCCUPANCY[offset]["lower"] = DIAGONAL_OCCUPANCY[offset]["lineEx"].And((1ULL << (offset + 1)) - 1);
      DIAGONAL_OCCUPANCY[offset]["upper"] = DIAGONAL_OCCUPANCY[offset]["lineEx"].And(~((1ULL << (offset + 1)) - 1));

      n = 0x0102040810204080;
      shift = (8-file)-(rank-1);

      if (shift > 0) {
        n >>= shift;
        n &= (0x8000000000000000 >> (8 * shift)) - 1;
      } else if (shift < 0) {
        n <<= -shift;
        n &= ~((0x0000000000000001ULL << (8 * (-shift) + 7)) - 1);
      }

      n ^= (0x0000000000000001ULL << (8 * (rank - 1) + (file - 1)));


      ANTIDIAGONAL_OCCUPANCY[offset]["lineEx"].SetAll(n);
      ANTIDIAGONAL_OCCUPANCY[offset]["lower"] = ANTIDIAGONAL_OCCUPANCY[offset]["lineEx"].And((1ULL << (offset + 1)) - 1);
      ANTIDIAGONAL_OCCUPANCY[offset]["upper"] = ANTIDIAGONAL_OCCUPANCY[offset]["lineEx"].And(~((1ULL << (offset + 1)) - 1));

      // PAWN MOVES

      PAWN_MOVES[offset]["white"].Set(offset + 8);
      PAWN_MOVES[offset]["black"].Set(offset - 8);

      // PAWN DOUBLE MOVES

      if (rank == 2) {
        PAWN_DOUBLE_MOVES[offset]["white"].Set(offset + 16);
      }

      if (rank == 7) {
        PAWN_DOUBLE_MOVES[offset]["black"].Set(offset - 16);
      }
      

      // PAWN CAPUTRES

      if (file != 0) {
        PAWN_CAPTURES[offset]["white"].Set(offset + 7);
        PAWN_CAPTURES[offset]["black"].Set(offset - 9);
      }

      if (file != 8) {
        PAWN_CAPTURES[offset]["white"].Set(offset + 9);
        PAWN_CAPTURES[offset]["black"].Set(offset - 7);
      }

      // KNIGHT MOVES

      if (file > 1) {
        if (rank > 2) {
          KNIGHT_MOVES[offset].Set(offset - 17);
        }
        if (rank < 7) {
          KNIGHT_MOVES[offset].Set(offset + 15);
        }

        if (file > 2) {
          if (rank > 1) {
            KNIGHT_MOVES[offset].Set(offset - 10);
          }
          if (rank < 8) {
            KNIGHT_MOVES[offset].Set(offset + 6);
          }
        }
      }

      if (file < 8) {
        if (rank > 2) {
          KNIGHT_MOVES[offset].Set(offset - 15);
        }
        if (rank < 7) {
          KNIGHT_MOVES[offset].Set(offset + 17);
        }

        if (file < 7) {
          if (rank > 1) {
            KNIGHT_MOVES[offset].Set(offset - 6);
          }
          if (rank < 8) {
            KNIGHT_MOVES[offset].Set(offset + 10);
          }
        }
      }
    }
  }
}

Bitmap* Board::GetLegalMoves(Piece* p) {
  unsigned int offset = Util::OffsetFromRF(p->Rank(), p->File());

  if (hasLegalMovesCacheFor[offset]) {
    return &cachedLegalMoves[offset];
  }

  Bitmap* moves = new Bitmap();

  if (p->Is(PAWN)) {
    GetPawnMoves(p, moves);
  } else if (p->Is(KNIGHT)) {
    GetKnightMoves(p, moves);
  } else if (p->Is(ROOK)) {
    GetRookMoves(p, moves);
  } else if(p->Is(BISHOP)) {
    GetBishopMoves(p, moves);
  } else if (p->Is(QUEEN)) {
    GetQueenMoves(p, moves);
  }

  // TODO: King moves

  cachedLegalMoves[offset] = *moves;
  hasLegalMovesCacheFor[offset] = true;

  return moves;
}

Bitmap Board::LineAttacks(BitmapCollection* pMask) {
  uint64_t lower = (*pMask)["lower"].And(pieces.All()).Bits();
  uint64_t upper = (*pMask)["upper"].And(pieces.All()).Bits();

  unsigned long int index;
  _BitScanReverse64(&index, lower | 1);

  uint64_t mMs1b = (int64_t)(-1) << index;
  uint64_t ls1b = upper & -upper;
  uint64_t odiff = 2 * ls1b + mMs1b;

  return (*pMask)["lineEx"].And(odiff);
}

void Board::GetQueenMoves(Piece* p, Bitmap* b) {
  int rank = p->Rank();
  int file = p->File();

  uint64_t offset = Util::OffsetFromRF(rank, file);
  PieceName pieceName = p->GetTypeAsString();

  b->OrPlace(LineAttacks(&DIAGONAL_OCCUPANCY[offset]));
  b->OrPlace(LineAttacks(&ANTIDIAGONAL_OCCUPANCY[offset]));
  b->OrPlace(LineAttacks(&FILE_OCCUPANCY[offset]));
  b->OrPlace(LineAttacks(&RANK_OCCUPANCY[offset]));

  b->AndPlace(pieces[pieceName.color].Inverse());
}

void Board::GetBishopMoves(Piece* p, Bitmap* b) {
  int rank = p->Rank();
  int file = p->File();

  uint64_t offset = Util::OffsetFromRF(rank, file);
  PieceName pieceName = p->GetTypeAsString();

  b->OrPlace(LineAttacks(&DIAGONAL_OCCUPANCY[offset]));
  b->OrPlace(LineAttacks(&ANTIDIAGONAL_OCCUPANCY[offset]));

  b->AndPlace(pieces[pieceName.color].Inverse());
}

void Board::GetRookMoves(Piece* p, Bitmap* b) {
  int rank = p->Rank();
  int file = p->File();

  uint64_t offset = Util::OffsetFromRF(rank, file);
  PieceName pieceName = p->GetTypeAsString();

  b->OrPlace(LineAttacks(&FILE_OCCUPANCY[offset]));
  b->OrPlace(LineAttacks(&RANK_OCCUPANCY[offset]));

  b->AndPlace(pieces[pieceName.color].Inverse());
}

void Board::GetKnightMoves(Piece* p, Bitmap* b) {
  int rank = p->Rank();
  int file = p->File();

  uint64_t offset = Util::OffsetFromRF(rank, file);

  std::string inverseColor = p->Is(WHITE_PIECE) ? "black" : "white";

  PieceName pieceName = p->GetTypeAsString();

  b->OrPlace(KNIGHT_MOVES[offset].And(pieces[pieceName.color].Inverse()));
}

void Board::GetPawnMoves(Piece* p, Bitmap* b) {
  int rank = p->Rank();
  int file = p->File();

  uint64_t offset = Util::OffsetFromRF(rank, file);

  std::string inverseColor = p->Is(WHITE_PIECE) ? "black" : "white";

  PieceName pieceName = p->GetTypeAsString();

  // One-step move

  b->OrPlace(PAWN_MOVES[offset][pieceName.color].And(pieces.All().Inverse()));

  if (!b->IsEmpty()) {
    // Two-step move
    b->OrPlace(PAWN_DOUBLE_MOVES[offset][pieceName.color].And(pieces.All().Inverse()));
  }

  // Captures
  b->OrPlace(PAWN_CAPTURES[offset][pieceName.color].And(pieces[inverseColor].Or(enPassantPawns)));
}

void Board::LoadFromFEN(std::string fen) {
  std::string pieceString, turnColorString, canCastleString, enPassantTargetSquareString, halfMoveClockString, fullMoveString;


  Util::FENParts(
    pieceString, 
    turnColorString,
    canCastleString,
    enPassantTargetSquareString,
    halfMoveClockString,
    fullMoveString,
    fen
  );

  int rank = 8;
  int file = 1;

  ImageReader* reader = new ImageReader();

  for (int i = 0; i < pieceString.size(); i++) {
    char c = pieceString[i];

    if (c == '/') {
      rank -= 1;
      file = 1;
      continue;
    }

    if(Util::IsCharNumOneToEight(c)) {
      file += Util::GetNumOneToEightFromChar(c);

      if(file == 9) {
        file = 1;
      }
      continue;
    }

    uint8_t type = Piece::GetPieceTypeFromChar(c);
    
    SetPieceAt(file, rank, type, reader);

    file++;
  }

  if (enPassantTargetSquareString != "-") {
    int r;
    int f;

    Util::RankFileFromString(enPassantTargetSquareString, &r, &f);

    enPassantPawns.Set(Util::OffsetFromRF(r, f));
  }

  turn = std::stoi(fullMoveString) * 2 - ((turnColorString[0] == 'w') ? 1 : 0);
}

unsigned int Board::Turn() {
  return turn;
}

bool Board::IsTurn(Piece* p) {
  return p->Is(WHITE_PIECE) == (turn % 2);
}

void Board::SetPieceAt(int file, int rank, uint8_t type, ImageReader* reader) {
  pieceAt[file - 1][rank - 1] = new Piece(type, rank, file, reader);

  unsigned int offset = Util::OffsetFromRF(rank, file);

  PieceName pieceName = pieceAt[file - 1][rank - 1]->GetTypeAsString();

  pieces[pieceName.color].Set(offset);

  piecesMap[pieceName.piece][pieceName.color].Set(offset);
}

bool Board::HasPieceAt(int rank, int file) {
  if (file < 1 || file > 8 || rank < 1 || rank > 8) {
    return false;
  }

  return pieces.Has(Util::OffsetFromRF(rank, file));
}

bool Board::HasPieceWithColorAt(int rank, int file, uint8_t color) {
  std::string colorName = (color == WHITE_PIECE) ? "white" : "black";

  if (file < 1 || file > 8 || rank < 1 || rank > 8) {
    return false;
  }

  return pieces[colorName].Has(Util::OffsetFromRF(rank, file));
}

bool Board::HasSpecificPieceAt(int rank, int file, PieceName p) {
  unsigned int offset = Util::OffsetFromRF(rank,file);

  if (file < 1 || file > 8 || rank < 1 || rank > 8) {
    return false;
  }

  if (p.color == "") {
    return piecesMap[p.piece].Has(offset);
  }

  return piecesMap[p.piece][p.color].Has(offset);
}

Piece* Board::PieceAt(int file, int rank) {
  return pieceAt[file - 1][rank - 1];
}

void Board::SetHighlight(Bitmap* b) {
  highlighted.OrPlace(b);
}

void Board::SetHighlight(int rank, int file) {
  highlighted.Set((rank - 1) * BOARD_SQUARE_WIDTH + (file - 1));
}

void Board::ClearHighlight() {
  highlighted.Clear();
}

void Board::SelectPieceAt(int rank, int file) {
  SetHighlight(rank, file);

  hasSelected = true;

  selectedPiece = PieceAt(file, rank);
}

bool Board::HasSelection() {
  return hasSelected;
}

void Board::Deselect() {
  hasSelected = false;
  selectedPiece = nullptr;
}

Piece* Board::SelectedPiece() {
  return selectedPiece;
}

void Board::MakeMove(Piece* p, int rank, int file) {
  // Check if is capture, update turn, update cache lists

  // TODO: Check if it is a double move (en passant-able)

  turn++;

  unsigned int newOffset = Util::OffsetFromRF(rank, file);

  if (HasPieceAt(rank, file)) {
    PieceName capturedPieceName = pieceAt[file-1][rank-1]->GetTypeAsString();

    piecesMap[capturedPieceName.piece][capturedPieceName.color].Clear(newOffset);

    delete pieceAt[file-1][rank-1];
  }

  int oldRank = p->Rank();
  int oldFile = p->File();

  unsigned int oldOffset = Util::OffsetFromRF(oldRank, oldFile);

  PieceName pieceName = p->GetTypeAsString();

  // Set pieces new file and rank
  p->Rank(rank);
  p->File(file);

  pieceAt[file-1][rank-1] = p;

  // Update bitmaps
  pieces[pieceName.color].Clear(oldOffset);
  pieces[pieceName.color].Set(newOffset);

  piecesMap[pieceName.piece][pieceName.color].Clear(oldOffset);
  piecesMap[pieceName.piece][pieceName.color].Set(newOffset);
}

bool Board::IsInBounds(int rank, int file) {
  if (rank < 1 || rank > 8 || file < 1 || file > 8) {
    return false;
  }

  return true;
}

bool Board::IsInBoundsAndCheckPiece(int rank, int file, bool wantsPiece) {
  if (!IsInBounds(rank, file)) {
    return false;
  }

  if (wantsPiece != HasPieceAt(rank, file)) {
    return false;
  }

  return true;
}