#include <move.h>
#include <constants.h>
#include <util.h>

Move::Move(int sR, int sF, int eR, int eF, bool isCap, bool isEnPas, bool isPawnDou) {
	startRank = sR;
	startFile = sF;

	endRank = eR;
	endFile = eF;

	isCapture = isCap;
	isEnPassant = isEnPas;
	isDoublePawn = isPawnDou;
}

Move::Move(int sR, int sF, int eR, int eF) : Move(sR, sF, eR, eF, false, false, false)  {}

Move::Move(int sR, int sF, int eR, int eF, bool isCapture) : Move(sR, sF, eR, eF, isCapture, false, false) {}

int Move::GetStartFile() {
	return startFile;
}

int Move::GetStartRank() {
	return startRank;
}

int Move::GetEndFile() {
	return endFile;
}

int Move::GetEndRank() {
	return endRank;
}

std::string Move::ToString() {
	return Util::RankFileToString(startRank, startFile) + Util::RankFileToString(endRank, endFile);
}