#ifndef UTIL_H
#define UTIL_H

#include <array>
#include <string>

namespace Util {
	void FENParts(std::string&, std::string&, std::string&, std::string&, std::string&, std::string&, std::string);

	bool IsCharNumOneToEight(char);
	int GetNumOneToEightFromChar(char);

	bool StringEndsWith(std::string, std::string);

	int RGBToHexInt(int, int, int);
}

#endif