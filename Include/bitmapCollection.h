#ifndef BITMAPCOLLECTION_H
#define BITMAPCOLLECTION_H

#include <bitmap.h>
#include <string>
#include <unordered_map>

class BitmapCollection {
private:
	std::unordered_map<std::string, Bitmap> bitmaps;

	int nMaps;

public:
	BitmapCollection();

	Bitmap& operator[] (std::string);

	bool Has(uint64_t);

	void Set(uint64_t);

	Bitmap& All();
};

#endif // !BITMAPCOLLECTION_H
