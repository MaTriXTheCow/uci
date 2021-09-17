#ifndef PIECE_H
#define PIECE_H

class Piece {
private:
  uint8_t descriptor;

public:
  Piece(uint8_t descriptor);

  void Draw();
}

#endif
