#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>

class Bitmap {
private:
  uint64_t bits;

public:
  Bitmap();
  Bitmap(uint64_t);

  uint64_t Bits();

  bool IsEmpty();

  bool Has(unsigned int);
  bool HasAll(unsigned int, unsigned int);
  bool HasAll(unsigned int, unsigned int, unsigned int);
  bool HasAll(unsigned int, unsigned int, unsigned int, unsigned int);
  bool HasAll(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

  void SetAll(uint64_t);

  void Set(unsigned int);
  void Set(Bitmap);
  void Set(Bitmap*);

  void AndPlace(Bitmap*);
  void AndPlace(Bitmap);
  void AndPlace(uint64_t);

  void OrPlace(Bitmap*);
  void OrPlace(Bitmap);
  void OrPlace(uint64_t);

  void XorPlace(Bitmap*);
  void XorPlace(Bitmap);
  void XorPlace(uint64_t);

  Bitmap Or(Bitmap*);
  Bitmap Or(Bitmap);
  Bitmap Or(uint64_t);

  Bitmap Xor(Bitmap*);
  Bitmap Xor(Bitmap);
  Bitmap Xor(uint64_t);

  Bitmap And(Bitmap*);
  Bitmap And(Bitmap);

  Bitmap Inverse();
  void InversePlace();

  void Clear();
  void Clear(unsigned int);
};

#endif
