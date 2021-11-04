#include <board.h>

#include <logger.h>
#include <piece.h>
#include <util.h>
#include <imageReader.h>

#include <array>

Board::Board() {
  Logger::Log("Board created");

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      pieceAt[i][j] = nullptr;
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

      unsigned int offset = (rank - 1) * BOARD_SQUARE_WIDTH + (file - 1);

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
}

Bitmap* Board::GetLegalMoves(Piece* p) {
  Bitmap* moves = new Bitmap();

  if (p -> Is(PAWN)) {
    GetPawnMoves(p, moves);
  } else if (p -> Is(ROOK)) {
    GetRookMoves(p, moves);
  } else if (p->Is(BISHOP)) {
    GetBishopMoves(p, moves);
  } else if (p->Is(KNIGHT)) {
    GetKnightMoves(p, moves);
  } else if (p->Is(QUEEN)) {
    GetQueenMoves(p, moves);
  } else {
    GetKingMoves(p, moves);
  }

  return moves;
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
}

void Board::SetPieceAt(int file, int rank, uint8_t type, ImageReader* reader) {
  pieceAt[file - 1][rank - 1] = new Piece(type, rank, file, reader);

  unsigned int offset = (rank-1) * BOARD_SQUARE_WIDTH + (file - 1);

  PieceName pieceName = pieceAt[file - 1][rank - 1]->GetTypeAsString();

  pieces[pieceName.color].Set(offset);

  piecesMap[pieceName.piece][pieceName.color].Set(offset);
}

bool Board::HasPieceAt(int rank, int file) {
  if (file < 1 || file > 8 || rank < 1 || rank > 8) {
    return false;
  }

  unsigned int offset = (rank-1) * BOARD_SQUARE_WIDTH + (file-1);

  return pieces.Has(offset);
}

bool Board::HasPieceWithColorAt(int rank, int file, uint8_t color) {
  std::string colorName = (color == WHITE_PIECE) ? "white" : "black";

  if (file < 1 || file > 8 || rank < 1 || rank > 8) {
    return false;
  }

  unsigned int offset = (rank - 1) * BOARD_SQUARE_WIDTH + (file - 1);

  return pieces[colorName].Has(offset);
}

bool Board::HasSpecificPieceAt(int rank, int file, PieceName p) {
  unsigned int offset = (rank - 1) * BOARD_SQUARE_WIDTH + (file - 1);

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
  highlighted.And(b);
}

void Board::SetHighlight(int rank, int file) {
  highlighted.Set((rank - 1) * BOARD_SQUARE_WIDTH + (file - 1));
}

void Board::ClearHighlight() {
  highlighted.Clear();
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