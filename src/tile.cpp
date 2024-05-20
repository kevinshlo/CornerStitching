#include "tile.hpp"

bool Pt::IsSize(const Pt& coord) const {
  return (0 < x) && (x <= (kLenMax - coord.x))  //
         && (0 < y) && (y <= (kLenMax - coord.y));
}

Tile::Cmp Tile::CmpX(Len x) {
  if (x < coord.x) return LT;
  if ((coord.x + size.x) <= x) return GT;
  return EQ;
}

Tile::Cmp Tile::CmpY(Len y) {
  if (y < coord.y) return LT;
  if ((coord.y + size.y) <= y) return GT;
  return EQ;
}
