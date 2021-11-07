#include <pieceMoves.h>

PieceMoves::PieceMoves(int movesLength) {
	nMoves = 0;
	moves = (Move*) malloc(sizeof(Move) * movesLength);
}

void PieceMoves::Add(Move m) {
	moves[nMoves] = m;
	nMoves++;
}

Move* PieceMoves::operator[] (int index) {
	return &moves[index];
}

int PieceMoves::Size() {
	return nMoves;
}