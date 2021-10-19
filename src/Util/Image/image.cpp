#include<image.h>
#include<logger.h>

#include<string>

int count = 1;

void Image::Fill(int color) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			pixels[i][j] = color;
		}
	}
}

void Image::SetPixelAt(int row, int col, int color) {
	pixels[row][col] = color;
}

int Image::GetPixelAt(int row, int col) {
	return pixels[row][col];
}

bool Image::IsTransparent(int color) {
	int r = color >> 16;
	int g = (color & 0x00FF00) >> 8;
	int b = color & 0x0000FF;

	return (r >= 100 && r <= 255 && g <= 30 && b >= 100 && b <= 255);
}

Image::Image() {
	OutputDebugString("Image created ");
	OutputDebugString(std::to_string(count).c_str());
	OutputDebugString("\n");

	count++;

	height = 80;
	width = 80;
}
