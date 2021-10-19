#include <imageReader.h>
#include <util.h>
#include <image.h>
#include <stdint.h>

#include <iostream>
#include <filesystem>

Image* ImageReader::ReadImage(std::string path) {
	if (cache.count(path)) {
		return cache[path];
	}


	Image* img = new Image();

	FILE* fp;
	errno_t err = fopen_s(&fp, path.c_str(), "rb");

	int row = 0;
	int col = 0;

	int nByte = 0;

	int c;
	do {
		c = getc(fp);

		nByte++;
	} while (nByte < 13);

	while (true) {
		int r = getc(fp);
		int g = getc(fp);
		int b = getc(fp);

		img->SetPixelAt(79-row, col, Util::RGBToHexInt(r, g, b));

		col++;

		if (col == 80) {
			row++;
			col %= 80;
		}

		if (row == 80) {
			break;
		}
	}

	fclose(fp);

	cache[path] = img;

	return img;
}