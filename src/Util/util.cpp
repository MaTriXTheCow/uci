#include <util.h>
#include <logger.h>

void Util::FENParts(std::string &pieces, std::string &turnColor, std::string &canCastle, std::string &enPassantTargetSquare, std::string &halfMove, std::string &fullMove, std::string fen) {
	int stringStartIndex = 0;
	int stringLen = 0;

	int partIndex = 0;

	std::string *parts[6] = {&pieces, &turnColor, &canCastle, &enPassantTargetSquare, &halfMove, &fullMove};

	for (int i = 0; i < fen.size(); i++) {
		char c = fen[i];

		if (c == ' ') {
			// Skip and save the string

			*parts[partIndex] = fen.substr(stringStartIndex, stringLen);

			stringStartIndex += stringLen + 1;
			partIndex++;

			stringLen = 0;

			continue;
		}

		stringLen++;
	}

	// For some reason last number will not come in
	fullMove = fen.back();
}

bool Util::IsCharNumOneToEight(char c) {
	return c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8';
}

int Util::GetNumOneToEightFromChar(char c) {
	switch (c) {
	case '1':
		return 1;
		break;
	case '2':
		return 2;
		break;
	case '3':
		return 3;
		break;
	case '4':
		return 4;
		break;
	case '5':
		return 5;
		break;
	case '6':
		return 6;
		break;
	case '7':
		return 7;
		break;
	case '8':
		return 8;
		break;
	}

	Logger::Log("GetNumOneToEightFromChar: Input:", c, "is invalid. Has to be in range 1-8");

	return -1;
}

int Util::RGBToHexInt(int r, int g, int b) {
	return (r << 16) | (g << 8) | (b);
}

bool Util::StringEndsWith(std::string s, std::string ending) {
	if (ending.size() > s.size()) {
		return false;
	}

	size_t i = s.size();
	size_t j = ending.size();

	while(i >= 0 && j >= 0) {
		if (s[i] != ending[j]) {
			return false;
		}

		if (i == 0 || j == 0) {
			break;
		}

		--i;
		--j;
	}

	return true;
}

std::string Util::RankFileToString(int rank, int file) {
	char fileC = (char)('a' + (rank - 1));
	char rankC = (char)('0' + (file - 1));

	std::string ret;

	ret += fileC;
	ret += rankC;

	return ret;
};