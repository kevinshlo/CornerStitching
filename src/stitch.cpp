#include "stitch.hpp"

#include <cassert>

Stitch::Stitch(const Pt& coord, const Pt& size) : coord_(coord), size_(size) {
  assert(coord_.QuadrantI() && "coord must lies in Quadrant I");
  assert(size_.IsSize() && "illegal size");
}

Id Stitch::PointFinding(const Pt& p, Id start) const {
  Id id = Exist(start) ? start : LastInserted();
  while (id != kNullId && !tiles_[id].value().Contain(p)) {
    // move up/down using rt/lb, until the tile's vertical range contains y
    while (id != kNullId) {
      assert(tiles_[id].has_value() && "id must point to a real tile");
      const auto& t = tiles_[id].value();
      auto cmp_y = t.CmpY(p);
      if (cmp_y == Tile::EQ)
        break;
      else
        id = (cmp_y == Tile::LT) ? t.lb : t.rt;
    }
    // move left/right using tr/bl, until the tile's horizontal range contains x
    while (id != kNullId) {
      assert(tiles_[id].has_value() && "id must point to a real tile");
      const auto& t = tiles_[id].value();
      auto cmp_x = t.CmpX(p);
      if (cmp_x == Tile::EQ)
        break;
      else
        id = (cmp_x == Tile::LT) ? t.bl : t.tr;
    }
    // repeat since misalignment might occur
  }
  return id;
}

Id Stitch::LastInserted() const {
  if (Exist(last_inserted_))
    return last_inserted_;
  else if (Size())
    for (int i = tiles_.size() - 1; i >= 0; i--)
      if (tiles_[i].has_value()) return i;
  return kNullId;
}
