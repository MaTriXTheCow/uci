#include <bitmap.h>

Bitmap::Bitmap() {
  bits = 0;
}

Bitmap::Bitmap(uint64_t n) {
  bits = n;
}

bool Bitmap::Has(unsigned int n) {
  return bits & (1ULL << n);
}

void Bitmap::Set(unsigned int n) {
  bits &= (1ULL << n) ^ 0xFFFFFFFFFFFFFFFFULL;

  bits |= (1ULL << n);
}

void Bitmap::SetAll(uint64_t n) {
  bits &= n ^ 0xFFFFFFFFFFFFFFFFULL;

  bits |= n;
}

void Bitmap::And(Bitmap* b) {
  bits &= b->bits;
}

void Bitmap::Clear() {
  bits = 0;
}
