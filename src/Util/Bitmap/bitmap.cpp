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

bool Bitmap::HasAll(unsigned int n1, unsigned int n2) {
  return bits & (1ULL << n1 | 1ULL << n2);
}

bool Bitmap::HasAll(unsigned int n1, unsigned int n2, unsigned int n3) {
  return bits & (1ULL << n1 | 1ULL << n2 | 1ULL << n3);
}

bool Bitmap::HasAll(unsigned int n1, unsigned int n2, unsigned int n3, unsigned int n4) {
  return bits & (1ULL << n1 | 1ULL << n2 | 1ULL << n3 | 1ULL << n4);
}

bool Bitmap::HasAll(unsigned int n1, unsigned int n2, unsigned int n3, unsigned int n4, unsigned int n5) {
  return bits & (1ULL << n1 | 1ULL << n2 | 1ULL << n3 | 1ULL << n4 | 1ULL << n5);
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

void Bitmap::XorPlace(Bitmap* b) {
  bits ^= b->bits;
}

void Bitmap::XorPlace(Bitmap b) {
  bits ^= b.bits;
}
void Bitmap::XorPlace(uint64_t n) {
  bits ^= n;
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

Bitmap Bitmap::Xor(Bitmap* b) {
  return bits ^ b->bits;
}

Bitmap Bitmap::Xor(Bitmap b) {
  return bits ^ b.bits;
}
Bitmap Bitmap::Xor(uint64_t n) {
  return bits ^ n;
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
