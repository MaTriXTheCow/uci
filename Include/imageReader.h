#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <pixel.h>
#include <image.h>
#include <string>
#include <map>

class ImageReader {
private:
  std::map<std::string, Image*> cache;

public:
  Image* ReadImage(std::string);
};

#endif
