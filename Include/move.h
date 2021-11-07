#ifndef MOVE_H
#define MOVE_H

#include <stdint.h>
#include <string>

class Move {
private:
	int startRank;
	int startFile;

	int endRank;
	int endFile;

	bool isCapture;
	bool isEnPassant;
	bool isDoublePawn;

public:
	Move(int, int, int, int);
	Move(int, int, int, int, bool);
	Move(int, int, int, int, bool, bool, bool);

	void Make();

	int GetStartRank();
	int GetStartFile();
	int GetEndRank();
	int GetEndFile();

	std::string ToString();
};
 
#endif // MOVE_H
