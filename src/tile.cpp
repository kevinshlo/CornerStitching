#include "tile.hpp"

Tile::Tile(const Pt& coord, const Pt& size, bool is_space)
    : coord(coord), size(size), is_space(is_space) {
  assert(coord.InQuadrantI() && "coord must lies in Quadrant I");
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
  assert(coord.InQuadrantI() && "coord must lies in Quadrant I");
  assert(size.IsSize(coord) && "illegal size");
}
