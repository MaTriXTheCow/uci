#ifndef IMAGE_H
#define IMAGE_H

class Image {
private:
  Pixel pixels[60][60];

public:
  Pixel GetPixelAt(int, int);
};

#endif
