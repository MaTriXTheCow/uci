#include <..\Util\Pixel\Pixel.h>
#include <..\Util\Image\image.h>

#ifndef IMAGEREADER_H
#define IMAGEREADER_H

class ImageReader {
private:

public:
  Image ReadImage(std::string);
};

#endif
