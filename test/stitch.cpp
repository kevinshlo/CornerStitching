#include "../src/stitch.hpp"

#include <gtest/gtest.h>

struct Example {
  typedef std::map<Id, std::vector<Id>> NeighborGolden;

  Stitch stitch;
  NeighborGolden right_neighbors;
  NeighborGolden left_neighbors;
  NeighborGolden top_neighbors;
  NeighborGolden bottom_neighbors;

  Example(Stitch&& stitch,  //
          NeighborGolden&& right_neighbors, NeighborGolden&& left_neighbors,
          NeighborGolden&& top_neighbors, NeighborGolden&& bottom_neighbors)
      : stitch(stitch),
        right_neighbors(right_neighbors),
        left_neighbors(left_neighbors),
        top_neighbors(top_neighbors),
        bottom_neighbors(bottom_neighbors) {}

  /* An example stitch from `examples/stitch1.drawio.png` */
  static Example Example1() {
    Stitch s(Pt(0, 0), Pt(30, 24));
    s.tiles_ = {
        Tile(Pt(0, 0), Pt(30, 2), kNullId, kNullId, kNullId, 4),
        Tile(Pt(0, 2), Pt(15, 3), kNullId, 0, 2, 7),
        Tile(Pt(15, 2), Pt(5, 5), 1, 0, 8, 9, false),
        Tile(Pt(20, 2), Pt(8, 2), 2, 0, 4, 8, false),
        Tile(Pt(28, 2), Pt(2, 2), 3, 0, kNullId, 8),
        Tile(Pt(0, 5), Pt(4, 8), kNullId, 1, 6, 13),
        Tile(Pt(4, 5), Pt(5, 8), 5, 1, 10, 13, false),
        Tile(Pt(9, 5), Pt(6, 2), 6, 1, 2, 9),
        Tile(Pt(20, 4), Pt(10, 3), 2, 3, kNullId, 9),
        Tile(Pt(9, 7), Pt(21, 4), 6, 7, kNullId, 12),
        Tile(Pt(9, 11), Pt(2, 2), 6, 9, 11, 13),
        Tile(Pt(11, 11), Pt(8, 4), 10, 9, 14, 16, false),
        Tile(Pt(19, 11), Pt(11, 3), 11, 9, kNullId, 15),
        Tile(Pt(0, 13), Pt(11, 2), kNullId, 5, 11, 16),
        Tile(Pt(19, 14), Pt(6, 4), 11, 12, 15, 19, false),
        Tile(Pt(25, 14), Pt(5, 4), 14, 12, kNullId, 19),
        Tile(Pt(0, 15), Pt(19, 3), kNullId, 13, 14, 19),
        Tile(Pt(0, 18), Pt(7, 4), kNullId, 16, 18, 20),
        Tile(Pt(7, 18), Pt(6, 4), 17, 16, 19, 20, false),
        Tile(Pt(13, 18), Pt(17, 4), 18, 16, kNullId, 20),
        Tile(Pt(0, 22), Pt(30, 2), kNullId, 17, kNullId, kNullId),
    };
    s.slots_ = {};
    s.last_inserted_ = 11;
    NeighborGolden right_neighbors = {
        {0, {}},    {1, {2}},        {2, {8, 3}}, {3, {4}},   {4, {}},
        {5, {6}},   {6, {10, 9, 7}}, {7, {2}},    {8, {}},    {9, {}},
        {10, {11}}, {11, {14, 12}},  {12, {}},    {13, {11}}, {14, {15}},
        {15, {}},   {16, {14}},      {17, {18}},  {18, {19}}, {19, {}},
        {20, {}},
    };
    NeighborGolden left_neighbors = {
        {0, {}},    {1, {}},        {2, {1, 7}}, {3, {2}},   {4, {3}},
        {5, {}},    {6, {5}},       {7, {6}},    {8, {2}},   {9, {6}},
        {10, {6}},  {11, {10, 13}}, {12, {11}},  {13, {}},   {14, {11, 16}},
        {15, {14}}, {16, {}},       {17, {}},    {18, {17}}, {19, {18}},
        {20, {}},
    };
    NeighborGolden top_neighbors = {
        {0, {4, 3, 2, 1}},  {1, {7, 6, 5}},    {2, {9}},   {3, {8}},
        {4, {8}},           {5, {13}},         {6, {13}},  {7, {9}},
        {8, {9}},           {9, {12, 11, 10}}, {10, {13}}, {11, {16}},
        {12, {15, 14}},     {13, {16}},        {14, {19}}, {15, {19}},
        {16, {19, 18, 17}}, {17, {20}},        {18, {20}}, {19, {20}},
        {20, {}},
    };
    NeighborGolden bottom_neighbors = {
        {0, {}},        {1, {0}},           {2, {0}},
        {3, {0}},       {4, {0}},           {5, {1}},
        {6, {1}},       {7, {1}},           {8, {3, 4}},
        {9, {7, 2, 8}}, {10, {9}},          {11, {9}},
        {12, {9}},      {13, {5, 6, 10}},   {14, {12}},
        {15, {12}},     {16, {13, 11}},     {17, {16}},
        {18, {16}},     {19, {16, 14, 15}}, {20, {17, 18, 19}},
    };
    return Example(std::move(s), std::move(right_neighbors),
                   std::move(left_neighbors), std::move(top_neighbors),
                   std::move(bottom_neighbors));
  }
};

TEST(Stitch, PointFinding1) {
  Example example = Example::Example1();
  const auto& s = example.stitch;
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
      const auto& t = s.tiles_[id].value();
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

enum Side { RIGHT, LEFT, TOP, BOTTOM };

void TestNeighborFinding(const Example& example, Side side) {
  const auto& s = example.stitch;
  // collect id of existing tiles
  std::vector<Id> ids;
  for (size_t i = 0; i < s.tiles_.size(); i++)
    if (s.tiles_[i].has_value()) ids.push_back(i);
  const auto& golden = side == RIGHT  ? example.right_neighbors
                       : side == LEFT ? example.left_neighbors
                       : side == TOP  ? example.top_neighbors
                                      : example.bottom_neighbors;
  // for each tile, find its neighbors
  for (auto id : ids) {
    auto neighbors = side == RIGHT  ? s.RightNeighborFinding(id)
                     : side == LEFT ? s.LeftNeighborFinding(id)
                     : side == TOP  ? s.TopNeighborFinding(id)
                                    : s.BottomNeighborFinding(id);
    EXPECT_EQ(golden.at(id), neighbors);
  }
}

TEST(Stitch, RightNeighborFinding1) {
  TestNeighborFinding(Example::Example1(), RIGHT);
}

TEST(Stitch, LeftNeighborFinding1) {
  TestNeighborFinding(Example::Example1(), LEFT);
}

TEST(Stitch, TopNeighborFinding1) {
  TestNeighborFinding(Example::Example1(), TOP);
}
TEST(Stitch, BottomNeighborFinding1) {
  TestNeighborFinding(Example::Example1(), BOTTOM);
}

TEST(Stitch, AreaSearch1) {
  Example example = Example::Example1();
  const auto& s = example.stitch;
  // collect id of existing tiles
  std::vector<Id> ids;
  for (size_t i = 0; i < s.tiles_.size(); i++)
    if (s.tiles_[i].has_value()) ids.push_back(i);
  // AreaSearch self should return self if solid
  for (auto id : ids) {
    Tile t = s.At(id).value();
    EXPECT_EQ(t.is_space ? kNullId : id, s.AreaSearch(t));
  }
  // Area Search the whole plane
  EXPECT_EQ(18, s.AreaSearch(Tile(s.coord_, s.size_ - Pt(0.1, 0.1))));
  // Custom cases
  EXPECT_EQ(6, s.AreaSearch({{0, 5}, {9, 8}}, 9));
  EXPECT_EQ(11, s.AreaSearch({{0, 2}, {19, 12}}, 4));
  EXPECT_EQ(14, s.AreaSearch({{15, 2}, {15, 16}}, 16));
  EXPECT_EQ(11, s.AreaSearch({{12, 12}, {1, 1}}, 9));
  EXPECT_EQ(2, s.AreaSearch({{9, 2}, {21, 5}}, 20));
  EXPECT_EQ(14, s.AreaSearch({{17, 13}, {13, 5}}));
}
