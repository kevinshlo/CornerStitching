#include "stitch.hpp"

#include <cassert>

Stitch::Stitch(const Pt& coord, const Pt& size) : coord_(coord), size_(size) {
  assert(coord_.QuadrantI() && "coord must lies in Quadrant I");
  assert(size_.IsSize() && "illegal size");
}

Id Stitch::PointFinding(const Pt& p) {
  auto id = FirstTile();
  while (id != kNullId && !tiles_[id].value().Contain(p)) {
    Tile& t = tiles_[id].value();
    // move up/down using rt/lb, until the tile's vertical range contains y
    for (auto cmp_y = t.CmpY(p); cmp_y != Tile::EQ;
         id = (cmp_y == Tile::LT) ? t.rt : t.lb);
    // move left/right using tr/bl, until the tile's horizontal range contains x
    for (auto cmp_x = t.CmpX(p); cmp_x != Tile::EQ;
         id = (cmp_x == Tile::LT) ? t.tr : t.bl);
    // repeat since misalignment might occur
  }
  return id;
}

Id Stitch::FirstTile() const {
  if (Size())
    for (size_t i = 0; i < tiles_.size(); i++)
      if (tiles_[i].has_value()) return i;
  return kNullId;
}
