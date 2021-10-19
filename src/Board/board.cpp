#include <board.h>

#include <logger.h>
#include <piece.h>
#include <util.h>
#include <imageReader.h>

#include <array>

Board::Board() {
  Logger::Log("Board created");

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

      if(HasPieceAt(rank, file)) {
        PieceAt(file, rank)->Draw(winProc);
      } else {
        winProc.FillPixelsSquare(rank, file, SQUARE_MEASUREMENT, squareColor);
      }
    }
  }
}

void Board::Init(std::string fen) {
  Logger::Log("Initialising board");

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      pieceAt[i][j] = nullptr;
    }
  }

  LoadFromFEN(fen);
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
  unsigned int offset = (rank-1) * BOARD_SQUARE_WIDTH + (file-1);

  return pieces.Has(offset);
}

bool Board::HasSpecificPieceAt(int rank, int file, PieceName p) {
  unsigned int offset = (rank - 1) * BOARD_SQUARE_WIDTH + (file - 1);

  if (p.color == "") {
    return piecesMap[p.piece].Has(offset);
  }

  return piecesMap[p.piece][p.color].Has(offset);
}

Piece* Board::PieceAt(int file, int rank) {
  return pieceAt[file - 1][rank - 1];
}
