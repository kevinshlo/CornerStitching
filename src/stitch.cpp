#include "stitch.hpp"

#include <cassert>

Id Stitch::PointFinding(const Pt& p) {
  auto id = FirstTile();
  while (id != kNullId && !tiles_[id].Contain(p)) {
    Tile& t = tiles_[id];
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

size_t Stitch::Size() const { return tiles_.size() - slots_.size(); }

Id Stitch::FirstTile() const {
  if (Size())
    for (size_t i = 0; i < tiles_.size(); i++)
      if (tiles_[i].Legal()) return i;
  return kNullId;
}

#ifdef GTEST
#include <gtest/gtest.h>

TEST(Stitch, dummy) { EXPECT_EQ(1 + 1, 2); }

#endif