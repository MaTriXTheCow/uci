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

void BitmapCollection::Set(uint64_t offset) {
	for (auto& pair : bitmaps) {
		pair.second.Set(offset);
	}
}

Bitmap& BitmapCollection::All() {
	Bitmap b = Bitmap();

	for (auto& pair : bitmaps) {
		b.OrPlace(pair.second);
	}

	return b;
}