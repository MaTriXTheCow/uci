#ifndef IMAGE_H
#define IMAGE_H

#include <pixel.h>

class Image {
private:
  int pixels[80][80];

public:
  int width;
  int height;

  Image();

  int GetPixelAt(int, int);
  void SetPixelAt(int, int, int);

  static bool IsTransparent(int);

  void Fill(int);
};

#endif
