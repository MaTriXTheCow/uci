#include <bitmap.h>

Bitmap::Bitmap() {
  bits = 0;
}

Bitmap::Bitmap(uint64_t n) {
  bits = n;
}

uint64_t Bitmap::Bits() {
  return bits;
}

bool Bitmap::IsEmpty() {
  return bits == 0;
}

bool Bitmap::Has(unsigned int n) {
  return bits & (1ULL << n);
}

void Bitmap::Set(unsigned int n) {
  bits &= (1ULL << n) ^ 0xFFFFFFFFFFFFFFFFULL;

  bits |= (1ULL << n);
}

void Bitmap::Set(Bitmap b) {
  int n = b.bits;

  bits &= (1ULL << n) ^ 0xFFFFFFFFFFFFFFFFULL;

  bits |= (1ULL << n);
}

void Bitmap::Set(Bitmap* b) {
  int n = b->bits;

  bits &= (1ULL << n) ^ 0xFFFFFFFFFFFFFFFFULL;

  bits |= (1ULL << n);
}

void Bitmap::SetAll(uint64_t n) {
  bits &= n ^ 0xFFFFFFFFFFFFFFFFULL;

  bits |= n;
}

void Bitmap::AndPlace(Bitmap* b) {
  bits &= b->bits;
}

void Bitmap::AndPlace(Bitmap b) {
  bits &= b.bits;
}


void Bitmap::AndPlace(uint64_t b) {
  bits &= b;
}

void Bitmap::OrPlace(Bitmap* b) {
  bits |= b->bits;
}

void Bitmap::OrPlace(Bitmap b) {
  bits |= b.bits;
}

void Bitmap::OrPlace(uint64_t n) {
  bits |= n;
}

Bitmap Bitmap::Or(Bitmap* b) {
  return bits | b->bits;
}

Bitmap Bitmap::Or(Bitmap b) {
  return bits | b.bits;
}

Bitmap Bitmap::Or(uint64_t n) {
  return bits | n;
}

Bitmap Bitmap::And(Bitmap* b) {
  return bits & b->bits;
}

Bitmap Bitmap::And(Bitmap b) {
  return bits & b.bits;
}

Bitmap Bitmap::Inverse() {
  return Bitmap(~bits);
}

void Bitmap::InversePlace() {
  bits = ~bits;
}

void Bitmap::Clear() {
  bits = 0;
}

void Bitmap::Clear(unsigned int offset) {
  bits &= ~(1ULL << offset);
}
