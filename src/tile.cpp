#include "tile.hpp"

Tile::Tile(const Pt& coord, const Pt& size, bool is_space)
    : coord(coord), size(size), is_space(is_space) {
  assert(coord.QuadrantI() && "coord must lies in Quadrant I");
  assert(size.IsSize(coord) && "illegal size");
}

Tile::Tile(const Pt& coord, const Pt& size, Id bl, Id lb, Id tr, Id rt,
           bool is_space)
    : coord(coord),
      size(size),
      bl(bl),
      lb(lb),
      tr(tr),
      rt(rt),
      is_space(is_space) {
  assert(coord.QuadrantI() && "coord must lies in Quadrant I");
  assert(size.IsSize(coord) && "illegal size");
}

bool Pt::IsSize(const Pt& coord) const {
  return (0 < x) && (x <= (kLenMax - coord.x))  //
         && (0 < y) && (y <= (kLenMax - coord.y));
}

Tile::Cmp Tile::CmpX(Len x) const {
  if (x < coord.x) return LT;
  if ((coord.x + size.x) <= x) return GT;
  return EQ;
}

Tile::Cmp Tile::CmpY(Len y) const {
  if (y < coord.y) return LT;
  if ((coord.y + size.y) <= y) return GT;
  return EQ;
}
