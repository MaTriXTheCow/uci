#include <piece.h>
#include <constants.h>
#include <logger.h>
#include <imageReader.h>
#include <pieceName.h>

Piece::Piece(uint8_t desc, uint8_t rnk, uint8_t fle, ImageReader* reader) {
  descriptor = desc;

  rank = rnk;
  file = fle;

  PieceName p = GetTypeAsString();

  pieceImg = reader -> ReadImage("src/Images/" + p.piece  + "_" + p.color + ".ppm");
}

bool Piece::Is(uint8_t t) {
  return descriptor & t;
}

void Piece::Draw(WindowProcess winProc) {
  winProc.DrawImageSquare(rank, file, pieceImg);
}

PieceName Piece::GetTypeAsString() {
  PieceName output;

  switch (descriptor & 0xFE) { // 0xFE is 00000001 in binary and zeroes the last bit (the color bit)
  case PAWN:
    output.piece = "pawn";
    break;

  case ROOK:
    output.piece = "rook";
    break;

  case BISHOP:
    output.piece = "bishop";
    break;

  case KNIGHT:
    output.piece = "knight";
    break;

  case QUEEN:
    output.piece = "queen";
    break;

  case KING:
    output.piece = "king";
    break;

  default:
    return output;
  }

  output.color = (descriptor & WHITE_PIECE) ? "white" : "black";

  return output;
}

uint8_t Piece::GetPieceTypeFromChar(char c) {
  switch (c) {
  case 'p':
    return BLACK_PIECE | PAWN;

  case 'P':
    return WHITE_PIECE | PAWN;

  case 'r':
    return BLACK_PIECE | ROOK;

  case 'R':
    return WHITE_PIECE | ROOK;

  case 'n':
    return BLACK_PIECE | KNIGHT;

  case 'N':
    return WHITE_PIECE | KNIGHT;

  case 'b':
    return BLACK_PIECE | BISHOP;

  case 'B':
    return WHITE_PIECE | BISHOP;

  case 'q':
    return BLACK_PIECE | QUEEN;

  case 'Q':
    return WHITE_PIECE | QUEEN;

  case 'k':
    return BLACK_PIECE | KING;

  case 'K':
    return WHITE_PIECE | KING;
  }

  Logger::Log("MakePieceFromChar: Char must be pPrRkKbBqQkK. Input", c, "not allowed.");

  return 0;
}
