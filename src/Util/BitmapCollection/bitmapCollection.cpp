#include <bitmapCollection.h>

BitmapCollection::BitmapCollection() {
	nMaps = 0;
}

Bitmap& BitmapCollection::operator[] (std::string name) {
	if (!bitmaps.count(name)) {
		bitmaps[name] = Bitmap();

		nMaps++;
	}

	return bitmaps[name];
}

bool BitmapCollection::Has(uint64_t offset) {
	for (auto& pair : bitmaps) {
		if (pair.second.Has(offset)) {
			return true;
		}
	}
		
	return false;
}