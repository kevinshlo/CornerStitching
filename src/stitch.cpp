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

#ifdef GTEST
#include <gtest/gtest.h>

std::vector<std::optional<Tile>> Tiles() {
  return {
      Tile(Pt(0, 0), Pt(30, 2), kNullId, kNullId, kNullId, 4),
      Tile(Pt(0, 2), Pt(15, 3), kNullId, 0, 2, 7),
      Tile(Pt(15, 2), Pt(5, 5), 1, 0, 8, 9),
      Tile(Pt(20, 2), Pt(8, 2), 2, 0, 4, 8),
      Tile(Pt(28, 2), Pt(2, 2), 3, 0, kNullId, 8),
      Tile(Pt(0, 5), Pt(4, 8), kNullId, 1, 6, 13),
      Tile(Pt(4, 5), Pt(5, 8), 5, 1, 10, 13),
      Tile(Pt(9, 5), Pt(6, 2), 6, 1, 2, 9),
      Tile(Pt(20, 4), Pt(10, 3), 2, 3, kNullId, 9),
      Tile(Pt(9, 7), Pt(21, 4), 6, 7, kNullId, 12),
      Tile(Pt(9, 11), Pt(2, 2), 6, 9, 11, 13),
      Tile(Pt(11, 11), Pt(8, 4), 10, 9, 14, 16),
      Tile(Pt(19, 11), Pt(11, 3), 11, 9, kNullId, 15),
      Tile(Pt(0, 13), Pt(11, 2), kNullId, 5, 11, 16),
      Tile(Pt(19, 14), Pt(6, 4), 11, 12, 15, 19),
      Tile(Pt(25, 14), Pt(5, 4), 14, 12, kNullId, 19),
      Tile(Pt(0, 15), Pt(19, 3), kNullId, 13, 14, 19),
      Tile(Pt(0, 18), Pt(7, 4), kNullId, 16, 18, 20),
      Tile(Pt(7, 18), Pt(6, 4), 17, 16, 19, 20),
      Tile(Pt(13, 18), Pt(17, 4), 18, 16, kNullId, 20),
      Tile(Pt(0, 22), Pt(30, 2), kNullId, 17, kNullId, kNullId),
  };
}

TEST(Stitch, PointFinding) {
  Stitch stitch;
  stitch.SetTiles(Tiles());
  stitch.SetSlots({});
}

#endif
