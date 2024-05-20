#include "../src/tile.hpp"

#include <gtest/gtest.h>

TEST(Tile, Cmp_Contain) {
  Len x = 1.1, y = 0.5, w = 2.3, h = 1.7;
  Pt coord(x, y), size(w, h);
  Tile t(coord, size);
  EXPECT_EQ(t.CmpX(coord - size), Tile::LT);
  EXPECT_EQ(t.CmpX(coord + size), Tile::GT);
  EXPECT_EQ(t.CmpX(coord + Pt(0.1, 0.1)), Tile::EQ);
  EXPECT_EQ(t.CmpY(coord - size), Tile::LT);
  EXPECT_EQ(t.CmpY(coord + size), Tile::GT);
  EXPECT_EQ(t.CmpY(coord + Pt(0.1, 0.1)), Tile::EQ);
  EXPECT_EQ(t.Contain(coord + size), false);
  EXPECT_EQ(t.Contain(coord), true);
}
