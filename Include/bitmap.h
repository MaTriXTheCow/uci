#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>

class Bitmap {
private:
  uint64_t bits;

public:
  Bitmap();
  Bitmap(uint64_t);

  bool Has(unsigned int);

  void SetAll(uint64_t);
  void Set(unsigned int);

  void And(Bitmap*);

  void Clear();
};

#endif
