#ifndef PIECEMOVES_H
#define PIECEMOVES_H

#include <move.h>

class PieceMoves {
private:
	Move* moves;

	int nMoves;
public:
	PieceMoves(int);

	void Add(Move);
	int Size();

	Move* operator[] (int);
};

#endif // !PIECEMOVES_H
