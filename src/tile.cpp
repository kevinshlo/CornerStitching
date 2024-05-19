#include "tile.hpp"

bool Pt::IsSize(const Pt& coord) const {
  return (0 < x) && (x < kLenMax - coord.x)  //
         && (0 < y) && (y < kLenMax - coord.y);
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

#ifdef GTEST
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

#endif
