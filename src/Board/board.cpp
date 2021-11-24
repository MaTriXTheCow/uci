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

  kingIsChecked = false;
  isDoubleCheck = false;

  hasKingPinCache = false;

  checkRayAndPiece = Bitmap(0);

  selectedPiece = nullptr;
  hasLegalMovesCacheFor.Clear();

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      pieceAt[i][j] = nullptr;
      cachedLegalMoves[i * 8 + j] = NULL;
    }
  }

  piecesMap["pawns"]   = BitmapCollection();
  piecesMap["rooks"]   = BitmapCollection();
  piecesMap["bishops"] = BitmapCollection();
  piecesMap["knights"] = BitmapCollection();
  piecesMap["queens"]  = BitmapCollection();
  piecesMap["kings"]   = BitmapCollection();

  castleMap[0] = false;
  castleMap[1] = false;
  castleMap[2] = false;
  castleMap[3] = false;

  blackKing = nullptr;
  whiteKing = nullptr;
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
  GenerateInBetweenMap();
}

void Board::GenerateInBetweenMap() {
  for (unsigned int offsetFrom = 0; offsetFrom < 64; offsetFrom++) {
    for (unsigned int offsetTo = 0; offsetTo < 64; offsetTo++) {
      if (offsetFrom == offsetTo) {
        continue;
      }

      if (offsetFrom < offsetTo) {
        continue;
      }

      // Check if not is a rank, file, diagonal or antidiagonal
      bool isSameFile = ((offsetFrom % 8) == (offsetTo % 8));
      bool isSameRank = ((offsetFrom / 8) == (offsetTo / 8));
      bool isSameDiag = ((offsetFrom % 8 - offsetFrom / 8) == (offsetTo % 8 - offsetTo / 8));
      bool isSameAntiDiag = ((offsetFrom / 8 + offsetFrom % 8) == (offsetTo % 8 + offsetTo / 8));

      if (!isSameFile && !isSameRank && !isSameDiag && !isSameAntiDiag) {
        continue;
      }



      if (isSameFile) {
        uint64_t n = 0x0101010101010101; // First file bitmap
        n <<= (offsetFrom % 8); // Shift to correct file

        n &= ~((1ULL << (offsetTo + 1)) - 1);
        n &= (1ULL << offsetFrom) - 1;

        IN_BETWEEN[offsetFrom][offsetTo].SetAll(n);
        IN_BETWEEN[offsetTo][offsetFrom].SetAll(n);

        continue;
      }

      if (isSameRank) {
        uint64_t n = 0x00000000000000FF;
        n <<= ((offsetFrom / 8ULL) * 8);

        n &= ~((1ULL << (offsetTo + 1)) - 1);
        n &= (1ULL << offsetFrom) - 1;

        IN_BETWEEN[offsetFrom][offsetTo].SetAll(n);
        IN_BETWEEN[offsetTo][offsetFrom].SetAll(n);

        continue;
      }

      if (isSameDiag) {
        uint64_t n = 0x8040201008040201;
        int shift = (offsetFrom%8) - (offsetFrom/8);

        if (shift > 0) {
          n <<= shift;
          n &= (0x8000000000000000 >> (8 * (shift - 1))) - 1;
        } else if (shift < 0) {
          n >>= -shift;
          n &= ~((0x0000000000000001ULL << (8 * (-shift - 1))) - 1);
        }

        n &= ~((1ULL << (offsetTo + 1)) - 1);
        n &= (1ULL << offsetFrom) - 1;

        IN_BETWEEN[offsetFrom][offsetTo].SetAll(n);
        IN_BETWEEN[offsetTo][offsetFrom].SetAll(n);

        continue;
      }

      if (isSameAntiDiag) {
        uint64_t n = 0x0102040810204080;
        int shift = (7 - offsetFrom % 8) - (offsetFrom / 8);

        if (shift > 0) {
          n >>= shift;
          n &= (0x8000000000000000 >> (8 * shift)) - 1;
        } else if (shift < 0) {
          n <<= -shift;
          n &= ~((0x0000000000000001ULL << (8 * (-shift) + 7)) - 1);
        }

        n &= ~((1ULL << (offsetTo + 1)) - 1);
        n &= (1ULL << offsetFrom) - 1;

        IN_BETWEEN[offsetFrom][offsetTo].SetAll(n);
        IN_BETWEEN[offsetTo][offsetFrom].SetAll(n);

        continue;
      }
    }
  }
}

void Board::GenerateMoveBitmaps() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      int rank = i + 1;
      int file = j + 1;

      uint64_t offset = Util::OffsetFromRF(rank, file);

      // Occupancy line bitmaps
      FILE_OCCUPANCY[offset]["lineEx"].SetAll((0x8080808080808080 ^ (0x8000000000000000 >> (8 * (8-rank)))) >> (8-file));
      FILE_OCCUPANCY[offset]["lower"] = FILE_OCCUPANCY[offset]["lineEx"].And(((1ULL << offset) - 1) | 1ULL << offset);
      FILE_OCCUPANCY[offset]["upper"] = FILE_OCCUPANCY[offset]["lineEx"].And(~(((1ULL << offset) - 1) | 1ULL << offset));

      RANK_OCCUPANCY[offset]["lineEx"].SetAll((0xFF00000000000000 ^ (0x8000000000000000 >> (8 - file))) >> (8*(8 - rank)));
      RANK_OCCUPANCY[offset]["lower"] = RANK_OCCUPANCY[offset]["lineEx"].And(((1ULL << offset) - 1) | 1ULL << offset);
      RANK_OCCUPANCY[offset]["upper"] = RANK_OCCUPANCY[offset]["lineEx"].And(~(((1ULL << offset) - 1) | 1ULL << offset));

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
      DIAGONAL_OCCUPANCY[offset]["lower"] = DIAGONAL_OCCUPANCY[offset]["lineEx"].And(((1ULL << offset) - 1) | 1ULL << offset);
      DIAGONAL_OCCUPANCY[offset]["upper"] = DIAGONAL_OCCUPANCY[offset]["lineEx"].And(~(((1ULL << offset) - 1) | 1ULL << offset));

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
      ANTIDIAGONAL_OCCUPANCY[offset]["lower"] = ANTIDIAGONAL_OCCUPANCY[offset]["lineEx"].And(((1ULL << offset) - 1) | 1ULL << offset);
      ANTIDIAGONAL_OCCUPANCY[offset]["upper"] = ANTIDIAGONAL_OCCUPANCY[offset]["lineEx"].And(~(((1ULL << offset) - 1) | 1ULL << offset));

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
      

      // PAWN CAPTURES

      if (file != 1) {
        if (rank != 8) {
          PAWN_CAPTURES[offset]["white"].Set(offset + 7);
        }
        
        if (rank != 1) {
          PAWN_CAPTURES[offset]["black"].Set(offset - 9);
        }
      }

      if (file != 8) {
        if (rank != 8) {
          PAWN_CAPTURES[offset]["white"].Set(offset + 9);
        }
        
        if (rank != 1) {
          PAWN_CAPTURES[offset]["black"].Set(offset - 7);
        }
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

      // KING MOVES

      if (file > 1) {
        KING_MOVES[offset].Set(offset - 1);

        if (rank > 1) {
          KING_MOVES[offset].Set(offset - 9);
          KING_MOVES[offset].Set(offset - 8);
        }

        if (rank < 8) {
          KING_MOVES[offset].Set(offset + 7);
          KING_MOVES[offset].Set(offset + 8);
        }
      }

      if (file < 8) {
        KING_MOVES[offset].Set(offset + 1);

        if (rank > 1) {
          KING_MOVES[offset].Set(offset - 7);
          KING_MOVES[offset].Set(offset - 8);
        }

        if (rank < 8) {
          KING_MOVES[offset].Set(offset + 9);
          KING_MOVES[offset].Set(offset + 8);
        }
      }
    }
  }
}

Bitmap* Board::GetAttacks(Piece* p) {
  unsigned int offset = Util::OffsetFromRF(p->Rank(), p->File());

  if (hasLegalMovesCacheFor.Has(offset)) {
    return cachedLegalMoves[offset];
  }

  cachedLegalMoves[offset] = new Bitmap();

  if (p->Is(PAWN)) {
    PawnAttacks(p, cachedLegalMoves[offset]);
  } else if (p->Is(KNIGHT)) {
    KnightAttacks(p, cachedLegalMoves[offset]);
  } else if (p->Is(ROOK)) {
    RookAttacks(p, cachedLegalMoves[offset]);
  } else if(p->Is(BISHOP)) {
    BishopAttacks(p, cachedLegalMoves[offset]);
  } else if (p->Is(QUEEN)) {
    QueenAttacks(p, cachedLegalMoves[offset]);
  } else if (p->Is(KING)) {
    KingAttacks(p, cachedLegalMoves[offset]);
  }

  hasLegalMovesCacheFor.Set(offset);

  return cachedLegalMoves[offset];
}

Bitmap* Board::GetMoves(Piece* p) {
  // TODO:
  // - Cache this, also cache the attacks
  //

  unsigned int offset = Util::OffsetFromRF(p->Rank(), p->File());
  Bitmap* ret = new Bitmap();

  std::string color        = p->Is(WHITE_PIECE) ? "white" : "black";
  std::string inverseColor = p->Is(WHITE_PIECE) ? "black" : "white";

  // Preparing for pin checks
  Piece* king = GetKing(color);
  Bitmap pinned = KingPins(king);

  unsigned int kingOffset = Util::OffsetFromRF(king->Rank(), king->File());

  if (p->Is(PAWN)) {
    PawnMoves(p, ret);

    if (!(ret->And(enPassantPawns).IsEmpty()) && !(RANK_OCCUPANCY[kingOffset]["lineEx"].And(1ULL << offset).IsEmpty())) {
      // En passant capture exists on the same rank as the king, now check for double en-passant pins
      Bitmap piecesEnPassantRemoved = pieces.All().Xor((p->Descriptor() & 1) ? enPassantPawns.Bits() >> 8 : enPassantPawns.Bits() << 8);
      
      Bitmap attacks = LineAttacks(&RANK_OCCUPANCY[kingOffset]);
      attacks.XorPlace(LineAttacks(piecesEnPassantRemoved.Xor(pieces[color].And(attacks)), &RANK_OCCUPANCY[kingOffset]));

      uint64_t pinner = attacks.And(PieceBitmap("rook", inverseColor)->Or(PieceBitmap("queen", inverseColor)).Bits()).Bits();

      unsigned long index;
      Bitmap pinned;

      while (pinner) {
        _BitScanForward64(&index, pinner);

        Bitmap* piecesColor = PiecesBitmap(p->Descriptor() & 1);
        Bitmap inBetweeners = IN_BETWEEN[index][kingOffset];

        pinned.OrPlace(inBetweeners.And(piecesColor));
        pinner &= pinner - 1;
      }

      if (pinned.Has(offset)) {
        ret->XorPlace(enPassantPawns);
      }
    }
  } else if (p->Is(KNIGHT)) {
    KnightMoves(p, ret);
  } else if (p->Is(ROOK)) {
    RookMoves(p, ret);
  } else if (p->Is(BISHOP)) {
    BishopMoves(p, ret);
  } else if (p->Is(QUEEN)) {
    QueenMoves(p, ret);
  } else if (p->Is(KING)) {
    KingMoves(p, ret);
  }

  // IF double check -> only move possible is a king move
  // If normal check -> Block/Capture

  if (kingIsChecked) {
    if (isDoubleCheck && !p->Is(KING)) {
      ret->Clear();
      return ret;
    } else if (!p->Is(KING)){
      ret->AndPlace(checkRayAndPiece);
    } else {
      // King cannot stay in the x-rayed line
      int8_t Rd = ((king->Rank() - checkerPiece->Rank() == 0) ? 0 : ((king->Rank() - checkerPiece->Rank() < 0) ? 1 : -1));
      int8_t Fd = ((king->File() - checkerPiece->File() == 0) ? 0 : ((king->File() - checkerPiece->File() < 0) ? 1 : -1));

      unsigned int checkerOffset = Util::OffsetFromRF(checkerPiece->Rank(), checkerPiece->File());

      int index = index = 2 + ((Rd * Fd + 1) / 2);

      if (Rd * Fd == 0) {
        index = abs(Rd);
      }

      BitmapCollection throughBms[4] = {FILE_OCCUPANCY[checkerOffset], RANK_OCCUPANCY[checkerOffset], ANTIDIAGONAL_OCCUPANCY[checkerOffset], DIAGONAL_OCCUPANCY[checkerOffset] };

      ret->AndPlace(throughBms[index]["lineEx"].Inverse());
    }
  }

  // If piece is pinned, only keep the moves on the same line
  if (pinned.Has(offset)) {
    // Piece is pinned
    int8_t Rd = ((king->Rank() - p->Rank() == 0) ? 0 : ((king->Rank() - p->Rank() < 0) ? 1 : -1));
    int8_t Fd = ((king->File() - p->File() == 0) ? 0 : ((king->File() - p->File() < 0) ? 1 : -1));

    int index = index = 2 + ((Rd * Fd + 1) / 2);

    if (Rd * Fd == 0) {
      index = abs(Rd);
    }

    BitmapCollection throughBms[4] = {FILE_OCCUPANCY[offset], RANK_OCCUPANCY[offset], ANTIDIAGONAL_OCCUPANCY[offset], DIAGONAL_OCCUPANCY[offset]};

    ret->AndPlace(throughBms[index]["lineEx"]);
  }

  return ret;
}

Bitmap Board::KingPins(Piece* king) {
  int rank = king->Rank();
  int file = king->File();

  unsigned int offset = Util::OffsetFromRF(rank, file);

  PieceName pieceName = king->GetTypeAsString();
  std::string inverseColor = king->Is(WHITE_PIECE) ? "black" : "white";

  Bitmap pinned;
  uint64_t pinner = XrayRookAttacks(pieces[pieceName.color], offset).Bits();
  pinner &= piecesMap["rook"][inverseColor].Or(piecesMap["queen"][inverseColor]).Bits();

  unsigned long index = 0;

  while (pinner) {
    _BitScanForward64(&index, pinner);

    pinned.OrPlace(IN_BETWEEN[index][offset].And(pieces[pieceName.color]));
    pinner &= pinner - 1;
  }

  pinner = XrayBishopAttacks(pieces[pieceName.color], offset).And(piecesMap["bishop"][inverseColor].Or(piecesMap["queen"][inverseColor])).Bits();

  while (pinner) {
    _BitScanForward64(&index, pinner);

    pinned.OrPlace(IN_BETWEEN[index][offset].And(pieces[pieceName.color]));
    pinner &= pinner - 1;
  }

  return pinned;
}

Bitmap Board::XrayRookAttacks(Bitmap blockers, unsigned int rookOffset) {
  Bitmap attacks;

  attacks.OrPlace(LineAttacks(&FILE_OCCUPANCY[rookOffset]));
  attacks.OrPlace(LineAttacks(&RANK_OCCUPANCY[rookOffset]));
  attacks.XorPlace(LineAttacks(pieces.All().Xor(blockers.And(attacks)), &FILE_OCCUPANCY[rookOffset]).Or(LineAttacks(pieces.All().Xor(blockers.And(attacks)), &RANK_OCCUPANCY[rookOffset])));

  return attacks;
}

Bitmap Board::XrayBishopAttacks(Bitmap blockers, unsigned int bishopOffset) {
  Bitmap attacks;

  attacks.OrPlace(LineAttacks(&DIAGONAL_OCCUPANCY[bishopOffset]));
  attacks.OrPlace(LineAttacks(&ANTIDIAGONAL_OCCUPANCY[bishopOffset]));
  attacks.XorPlace(LineAttacks(pieces.All().Xor(blockers.And(attacks)), &DIAGONAL_OCCUPANCY[bishopOffset]).Or(LineAttacks(pieces.All().Xor(blockers.And(attacks)), &ANTIDIAGONAL_OCCUPANCY[bishopOffset])));

  return attacks;
}

Bitmap Board::LineAttacks(Bitmap occ, BitmapCollection* pMask) {
  uint64_t lower = (*pMask)["lower"].And(occ).Bits();
  uint64_t upper = (*pMask)["upper"].And(occ).Bits();

  unsigned long int index;
  _BitScanReverse64(&index, lower | 1);

  uint64_t mMs1b = (int64_t)(-1) << index;
  uint64_t ls1b = upper & -upper;
  uint64_t odiff = 2 * ls1b + mMs1b;

  return (*pMask)["lineEx"].And(odiff);
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

Piece* Board::GetKing(std::string color) {
  return (color == "white") ? whiteKing : blackKing;
}

Bitmap* Board::GetAttacksFromColor(uint8_t color) {
  Bitmap* attacks = new Bitmap();

  uint64_t colorPiecesBits = PiecesBitmap(color)->Bits();

  unsigned long int index;

  while (colorPiecesBits > 0) {
    _BitScanForward64(&index, colorPiecesBits);

    int rank = (index / 8) + 1;
    int file = (index % 8) + 1;

    Piece* attacksFromP = PieceAt(file, rank);

    attacks -> OrPlace(GetAttacks(attacksFromP));

    colorPiecesBits &= colorPiecesBits - 1;
  }

  return attacks;
}

void Board::KingMoves(Piece* p, Bitmap* b) {
  unsigned int offset = Util::OffsetFromRF(p->Rank(), p->File());

  KingAttacks(p, b);

  uint8_t inverseColor = (p->Descriptor() & 1) ^ 1;

  // ADD CASTLING AND CHECK CHECKING
  Bitmap* attacks = GetAttacksFromColor(inverseColor);

  b->AndPlace(attacks->Inverse());

  b->AndPlace(PiecesBitmap(p->Descriptor() & 1) -> Inverse());

  // Castling
  if (castleMap[inverseColor * 2] && IN_BETWEEN[offset][offset + 3].And(pieces.All()).IsEmpty() && IN_BETWEEN[offset][offset+2].And(attacks).IsEmpty()) {
    // Kingside castle is OK!
    b->Set(offset + 2ULL);
  }

  if (castleMap[inverseColor * 2 + 1] && IN_BETWEEN[offset][offset - 4].And(pieces.All()).IsEmpty() && IN_BETWEEN[offset][offset-2].And(attacks).IsEmpty()) {
    // Queenside castle is OK!
    b->Set(offset - 2ULL);
  }
}

void Board::QueenMoves(Piece* p, Bitmap* b) {
  QueenAttacks(p, b);

  b->AndPlace(PiecesBitmap(p->Descriptor() & 1) -> Inverse());
}

void Board::RookMoves(Piece* p, Bitmap* b) {
  RookAttacks(p, b);

  b->AndPlace(PiecesBitmap(p->Descriptor() & 1) -> Inverse());
}

void Board::BishopMoves(Piece* p, Bitmap* b) {
  BishopAttacks(p, b);

  b->AndPlace(PiecesBitmap(p->Descriptor() & 1) -> Inverse());
}

void Board::KnightMoves(Piece* p, Bitmap* b) {
  KnightAttacks(p, b);

  b->AndPlace(PiecesBitmap(p->Descriptor() & 1) -> Inverse());
}

void Board::PawnMoves(Piece* p, Bitmap* b) {
  PawnAttacks(p, b);

  unsigned int offset = Util::OffsetFromRF(p->Rank(), p->File());
  PieceName pieceName = p->GetTypeAsString();

  // Removing if there are no pieces to attack.
  b->AndPlace(PiecesBitmap((p->Descriptor() & 1) ^ 1)->Or(enPassantPawns));

  // Add double moves
  b->OrPlace(PAWN_MOVES[offset][pieceName.color].And(PiecesBitmap() -> Inverse()));

  // If first move completed, try adding next move
  if (!(b->And(PAWN_MOVES[offset][pieceName.color])).IsEmpty()) {
    b->OrPlace(PAWN_DOUBLE_MOVES[offset][pieceName.color].And(PiecesBitmap()->Inverse()));
  }
}

void Board::KingAttacks(Piece* p, Bitmap* b) {
  int rank = p->Rank();
  int file = p->File();

  uint64_t offset = Util::OffsetFromRF(rank, file);

  b->OrPlace(KING_MOVES[offset]);
}

void Board::QueenAttacks(Piece* p, Bitmap* b) {
  int rank = p->Rank();
  int file = p->File();

  uint64_t offset = Util::OffsetFromRF(rank, file);

  b->OrPlace(LineAttacks(&DIAGONAL_OCCUPANCY[offset]));
  b->OrPlace(LineAttacks(&ANTIDIAGONAL_OCCUPANCY[offset]));
  b->OrPlace(LineAttacks(&FILE_OCCUPANCY[offset]));
  b->OrPlace(LineAttacks(&RANK_OCCUPANCY[offset]));
}

void Board::BishopAttacks(Piece* p, Bitmap* b) {
  int rank = p->Rank();
  int file = p->File();

  uint64_t offset = Util::OffsetFromRF(rank, file);

  b->OrPlace(LineAttacks(&DIAGONAL_OCCUPANCY[offset]));
  b->OrPlace(LineAttacks(&ANTIDIAGONAL_OCCUPANCY[offset]));
}

void Board::RookAttacks(Piece* p, Bitmap* b) {
  int rank = p->Rank();
  int file = p->File();

  uint64_t offset = Util::OffsetFromRF(rank, file);

  b->OrPlace(LineAttacks(&FILE_OCCUPANCY[offset]));
  b->OrPlace(LineAttacks(&RANK_OCCUPANCY[offset]));
}

void Board::KnightAttacks(Piece* p, Bitmap* b) {
  int rank = p->Rank();
  int file = p->File();

  uint64_t offset = Util::OffsetFromRF(rank, file);

  b->OrPlace(KNIGHT_MOVES[offset]);
}

void Board::PawnAttacks(Piece* p, Bitmap* b) {
  int rank = p->Rank();
  int file = p->File();

  uint64_t offset = Util::OffsetFromRF(rank, file);

  PieceName pieceName = p->GetTypeAsString();

  // Captures
  b->OrPlace(PAWN_CAPTURES[offset][pieceName.color]);
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

  if (canCastleString != "-") {
    int length = canCastleString.length();

    for (int i = 0; i < length; i++) {
      char c = canCastleString[i];

      if (c == 'K') {
        castleMap[0] = true;
      } else if (c == 'Q') {
        castleMap[1] = true;
      } else if (c == 'k') {
        castleMap[2] = true;
      } else {
        castleMap[3] = true;
      }
    }
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

  if (pieceName.piece == "king") {
    if (pieceName.color == "white") {
      whiteKing = pieceAt[file - 1][rank - 1];
    } else {
      blackKing = pieceAt[file - 1][rank - 1];
    }
  }
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

Bitmap* Board::PiecesBitmap(uint8_t color) {
  return &pieces[color ? "white" : "black"];
}

Bitmap* Board::PieceBitmap(std::string type, std::string color) {
  return &piecesMap[type][color];
}

Bitmap* Board::PiecesBitmap() {
  return &pieces.All();
}

bool Board::HasEnPassantSquareAt(unsigned int offset) {
  return enPassantPawns.Has(offset);
}

void Board::MakeMove(Piece* p, int rank, int file) {
  // Check if is capture, update turn, update cache lists
  // 
  // TODO: Check if pawn is on last rank
  //     pause game
  //     prompt a change

  turn++;

  unsigned int newOffset = Util::OffsetFromRF(rank, file);


  // If a capture, remove the captured piece from all bitmaps and maps
  if (HasPieceAt(rank, file)) {
    PieceName capturedPieceName = pieceAt[file-1][rank-1]->GetTypeAsString();

    piecesMap[capturedPieceName.piece][capturedPieceName.color].Clear(newOffset);
    pieces[capturedPieceName.color].Clear(newOffset);

    delete pieceAt[file-1][rank-1];
  }

  int oldRank = p->Rank();
  int oldFile = p->File();

  unsigned int oldOffset = Util::OffsetFromRF(oldRank, oldFile);

  PieceName pieceName = p->GetTypeAsString();

  bool isEnPassantCaptureIfPawn = enPassantPawns.Has(newOffset);
  enPassantPawns.Clear();

  // Check for pawn double moves and en passants

  if (p->Is(PAWN)) {
    if (PAWN_DOUBLE_MOVES[oldOffset][pieceName.color].Has(newOffset)) {
      // It is a double move and can be en passanted next turn

      int pawnDirection = (p->Is(WHITE_PIECE) ? 1 : -1);
      unsigned int enPassantCaptureOffset = oldOffset + pawnDirection*8;

      enPassantPawns.Set(enPassantCaptureOffset);
    }

    if (isEnPassantCaptureIfPawn) {
      // It is an en passant capture

      int pawnDirection = (p->Is(WHITE_PIECE) ? 1 : -1);
      unsigned int enPassantCaptureOffset = newOffset - pawnDirection * 8;

      PieceName capturedPieceName = pieceAt[file - 1][rank - 1 - pawnDirection]->GetTypeAsString();

      piecesMap[capturedPieceName.piece][capturedPieceName.color].Clear(enPassantCaptureOffset);
      pieces[capturedPieceName.color].Clear(enPassantCaptureOffset);

      delete pieceAt[file - 1][rank - 1 - pawnDirection];
    }
  }

  if (p->Is(KING)) {
    if ((oldFile - file == 2 && castleMap[(oldRank / 8) * 2 + 1]) || (oldFile - file == -2 && castleMap[(oldRank / 8) * 2])) {
      // Castling

      int leftOrRight = (oldFile - file) / 2; // -1 if left 1 if right
      std::string color = (oldRank == 1) ? "white" : "black";

      pieceAt[file - 1 + leftOrRight][rank - 1] = pieceAt[(7 - leftOrRight * 7) / 2][rank - 1];
      pieceAt[(7 - leftOrRight * 7) / 2][rank - 1] = nullptr;

      pieceAt[file - 1 + leftOrRight][rank - 1]->File(file + leftOrRight);
      
      piecesMap["rook"][color].Clear((oldRank - 1) * 8 + (7 - leftOrRight * 7) / 2);
      piecesMap["rook"][color].Set(newOffset + leftOrRight);

      pieces[color].Clear((oldRank - 1) * 8 + (7 - leftOrRight * 7) / 2);
      pieces[color].Set(newOffset + leftOrRight);
    }

    castleMap[(oldRank / 8) * 2] = false;
    castleMap[(oldRank / 8) * 2 + 1] = false;
  }

  if (p->Is(ROOK)) {
    castleMap[(oldRank / 8)*2 + ((9 - oldFile) / 8)] = false;
  }

  // Set pieces new file and rank
  p->Rank(rank);
  p->File(file);

  pieceAt[file-1][rank-1] = p;
  pieceAt[oldFile - 1][oldRank - 1] = nullptr;

  // Update bitmaps
  pieces[pieceName.color].Clear(oldOffset);
  pieces[pieceName.color].Set(newOffset);

  piecesMap[pieceName.piece][pieceName.color].Clear(oldOffset);
  piecesMap[pieceName.piece][pieceName.color].Set(newOffset);

  // Check if king is checked, also detect CHECKMATE
  Piece* king = GetKing(pieceName.color == "white" ? "black" : "white");
  unsigned int kingOffset = Util::OffsetFromRF(king->Rank(), king->File());

  if (GetAttacksFromColor(p->Descriptor() & 1)->Has(kingOffset)) {
    kingIsChecked = true;

    uint64_t piecesNum = PiecesBitmap((king -> Descriptor() & 1) ^ 1) -> Bits();
    unsigned long index;

    bool hasOneCheck = false;

    while (piecesNum > 0) {
      _BitScanForward64(&index, piecesNum);

      int rank = index / 8;
      int file = index % 8;

      if (GetAttacks(pieceAt[file][rank])->Has(kingOffset)) {
        checkRayAndPiece = IN_BETWEEN[index][kingOffset];
        checkRayAndPiece.Set(index);
        checkerPiece = pieceAt[file][rank];

        isDoubleCheck = hasOneCheck;
        hasOneCheck = true;
      }

      piecesNum &= piecesNum - 1;
    }

  } else {
    kingIsChecked = false;
    isDoubleCheck = false;
  }

  hasLegalMovesCacheFor.Clear();
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

bool Board::KingChecked() {
  return kingIsChecked;
}