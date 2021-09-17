#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <..\Util\Pixel\Pixel.h>
#include <..\Util\Image\image.h>

class ImageReader {
private:

public:
  Image ReadImage(std::string);
};

#endif
