// Python wrapper of static functions in `test_stitch`

#ifdef PY

#pragma once

#include "../src/py.hpp"
#include "test_stitch.hpp"

// get the existing tiles
std::vector<PyTile> Tiles(PyStitch& s) {
  std::vector<PyTile> ret;
  for (auto id : TestStitch::Tiles(s.s_)) ret.push_back(PyTile(s.s_, id));
  return ret;
}
// get golden neighbors of tile by visiting all tiles
std::vector<PyTile> GoldenLeftNeighbors(PyTile& t) {
  std::vector<PyTile> ret;
  for (auto id : TestStitch::GoldenNeighbors(t.s_, t.id_, LEFT))
    ret.push_back(PyTile(t.s_, id));
  return ret;
}
std::vector<PyTile> GoldenBottomNeighbors(PyStitch& s, PyTile& t) {
  std::vector<PyTile> ret;
  for (auto id : TestStitch::GoldenNeighbors(t.s_, t.id_, BOTTOM))
    ret.push_back(PyTile(t.s_, id));
  return ret;
}
std::vector<PyTile> GoldenRightNeighbors(PyStitch& s, PyTile& t) {
  std::vector<PyTile> ret;
  for (auto id : TestStitch::GoldenNeighbors(t.s_, t.id_, RIGHT))
    ret.push_back(PyTile(t.s_, id));
  return ret;
}
std::vector<PyTile> GoldenTopNeighbors(PyStitch& s, PyTile& t) {
  std::vector<PyTile> ret;
  for (auto id : TestStitch::GoldenNeighbors(t.s_, t.id_, TOP))
    ret.push_back(PyTile(t.s_, id));
  return ret;
}
// get neighbors of tile
std::vector<PyTile> LeftNeighbors(PyStitch& s, PyTile& t) {
  std::vector<PyTile> ret;
  for (auto id : TestStitch::Neighbors(t.s_, t.id_, LEFT))
    ret.push_back(PyTile(t.s_, id));
  return ret;
}
std::vector<PyTile> BottomNeighbors(PyStitch& s, PyTile& t) {
  std::vector<PyTile> ret;
  for (auto id : TestStitch::Neighbors(t.s_, t.id_, BOTTOM))
    ret.push_back(PyTile(t.s_, id));
  return ret;
}
std::vector<PyTile> RightNeighbors(PyStitch& s, PyTile& t) {
  std::vector<PyTile> ret;
  for (auto id : TestStitch::Neighbors(t.s_, t.id_, RIGHT))
    ret.push_back(PyTile(t.s_, id));
  return ret;
}
std::vector<PyTile> TopNeighbors(PyStitch& s, PyTile& t) {
  std::vector<PyTile> ret;
  for (auto id : TestStitch::Neighbors(t.s_, t.id_, TOP))
    ret.push_back(PyTile(t.s_, id));
  return ret;
}

#define R_EXPECT_EQ(l, r)  \
  if ((l) != (r)) ret = 1; \
  EXPECT_EQ(l, r)
#define R_EXPECT_NE(l, r)  \
  if ((l) == (r)) ret = 1; \
  EXPECT_NE(l, r)
#define R_EXPECT_FALSE(e) \
  if (e) ret = 1;         \
  EXPECT_FALSE(e)
// check neighbors & pointers of all tiles
int CheckNeighbors(PyStitch& ps) {
  int ret = 0;
  const auto& s = ps.s_;
  for (auto id : TestStitch::Tiles(s)) {
    const auto& t = s.Ref(id);
    auto rights = TestStitch::GoldenNeighbors(s, id, RIGHT);
    R_EXPECT_EQ(rights, s.RightNeighborFinding(id));
    R_EXPECT_EQ(rights.size() ? rights.front() : kNullId, t.tr);
    auto lefts = TestStitch::GoldenNeighbors(s, id, LEFT);
    R_EXPECT_EQ(lefts, s.LeftNeighborFinding(id));
    R_EXPECT_EQ(lefts.size() ? lefts.front() : kNullId, t.bl);
    auto tops = TestStitch::GoldenNeighbors(s, id, TOP);
    R_EXPECT_EQ(tops, s.TopNeighborFinding(id));
    R_EXPECT_EQ(tops.size() ? tops.front() : kNullId, t.rt);
    auto bottoms = TestStitch::GoldenNeighbors(s, id, BOTTOM);
    R_EXPECT_EQ(bottoms, s.BottomNeighborFinding(id));
    R_EXPECT_EQ(bottoms.size() ? bottoms.front() : kNullId, t.lb);
  }
  return ret;
}
// check no overlap between tiles & whole plane is covered
int CheckTiles(PyStitch& ps) {
  int ret = 0;
  const auto& s = ps.s_;
  std::vector<Id> ids = TestStitch::Tiles(s);
  Len area = 0;
  for (auto x : ids) {
    area += s.Ref(x).size.x * s.Ref(x).size.y;
    for (auto y : ids) {
      R_EXPECT_EQ(x == y, s.Ref(x).Overlap(s.Ref(y)));
    }
  }
  R_EXPECT_EQ(s.size_.x * s.size_.y, area);
  return ret;
}
// check maximum horizontal strip property
bool CheckStrip(PyStitch& ps) {
  int ret = 0;
  const auto& s = ps.s_;
  for (auto id : TestStitch::Tiles(s)) {
    const auto& tl = s.Ref(id);
    if (tl.is_space) {
      if (s.Exist(tl.bl)) {
        R_EXPECT_FALSE(s.Ref(tl.bl).is_space) << id;
      }
      if (s.Exist(tl.tr)) {
        R_EXPECT_FALSE(s.Ref(tl.tr).is_space) << id;
      }
      // if (s.Exist(tl.lb) && s.Ref(tl.lb).is_space) {
      //   R_EXPECT_FALSE(s.Ref(tl.lb).coord.x == tl.coord.x &&
      //                  s.Ref(tl.lb).size.x == tl.size.x)
      //       << id;
      // }
      // if (s.Exist(tl.rt) && s.Ref(tl.rt).is_space) {
      //   R_EXPECT_FALSE(s.Ref(tl.rt).coord.x == tl.coord.x &&
      //                  s.Ref(tl.rt).size.x == tl.size.x)
      //       << id;
      // }
    }
  }
  return ret;
}

#endif