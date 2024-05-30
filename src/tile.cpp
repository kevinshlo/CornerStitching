#include "tile.hpp"

std::ostream& operator<<(std::ostream& o, const Pt& p) {
  o << "(" << p.x << "," << p.y << ")";
  return o;
}

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

std::ostream& operator<<(std::ostream& o, const Tile& t) {
  o << t.coord << "," << t.size                                              //
    << ",bl:" << t.bl << ",lb:" << t.lb << ",tr:" << t.tr << ",rt:" << t.rt  //
    << (t.is_space ? ",s" : "");
  return o;
}
