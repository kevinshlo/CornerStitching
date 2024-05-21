#include "../src/stitch.hpp"

#include <gtest/gtest.h>

/* An example stitch from `examples/stitch1.drawio.png` */
Stitch Example1() {
  Stitch s(Pt(0, 0), Pt(30, 24));
  s.tiles_ = {
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
  s.slots_ = {};
  s.last_inserted_ = 11;
  return s;
}

TEST(Stitch, PointFinding1) {
  Stitch s = Example1();
  // collect id of existing tiles
  std::vector<Id> ids;
  for (size_t i = 0; i < s.tiles_.size(); i++)
    if (s.tiles_[i].has_value()) ids.push_back(i);
  // starting tiles
  std::vector<Id> starts = {
      kNullId,                                             // default
      s.PointFinding(s.coord_),                            // bottom-left
      s.PointFinding(s.coord_ + Pt(s.size_.x - 0.1, 0)),   // bottom-right
      s.PointFinding(s.coord_ + Pt(0, s.size_.y - 0.1)),   // top-left
      s.PointFinding(s.coord_ + s.size_ + Pt(-0.1, -0.1))  // top-right
  };
  for (auto it = starts.begin() + 1; it != starts.end(); it++)
    EXPECT_NE(kNullId, *it);
  // for each starting tile, test each tile's all corners
  for (auto start : starts) {
    for (auto id : ids) {
      Tile &t = s.tiles_[id].value();
      EXPECT_EQ(id, s.PointFinding(t.coord, start));
      EXPECT_NE(id, s.PointFinding(t.coord + Pt(t.size.x, 0), start));
      EXPECT_EQ(id, s.PointFinding(t.coord + Pt(t.size.x - 0.1, 0), start));
      EXPECT_NE(id, s.PointFinding(t.coord + Pt(0, t.size.y), start));
      EXPECT_EQ(id, s.PointFinding(t.coord + Pt(0, t.size.y - 0.1), start));
      EXPECT_NE(id, s.PointFinding(t.coord + t.size, start));
      EXPECT_EQ(id, s.PointFinding(t.coord + t.size + Pt(-0.1, -0.1), start));
      EXPECT_EQ(id, s.PointFinding(t.coord + t.size / 2));
    }
  }
}
