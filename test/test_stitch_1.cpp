#include "test_stitch.hpp"

/* An example from `examples/stitch1.drawio.png` */
TestStitch Stitch1() {
  Stitch s({0, 0}, {30, 24});
  s.tiles_ = {
      Tile({0, 0}, {30, 2}, kNullId, kNullId, kNullId, 4),
      Tile({0, 2}, {15, 3}, kNullId, 0, 2, 7),
      Tile({15, 2}, {5, 5}, 1, 0, 8, 9, false),
      Tile({20, 2}, {8, 2}, 2, 0, 4, 8, false),
      Tile({28, 2}, {2, 2}, 3, 0, kNullId, 8),
      Tile({0, 5}, {4, 8}, kNullId, 1, 6, 13),
      Tile({4, 5}, {5, 8}, 5, 1, 10, 13, false),
      Tile({9, 5}, {6, 2}, 6, 1, 2, 9),
      Tile({20, 4}, {10, 3}, 2, 3, kNullId, 9),
      Tile({9, 7}, {21, 4}, 6, 7, kNullId, 12),
      Tile({9, 11}, {2, 2}, 6, 9, 11, 13),
      Tile({11, 11}, {8, 4}, 10, 9, 14, 16, false),
      Tile({19, 11}, {11, 3}, 11, 9, kNullId, 15),
      Tile({0, 13}, {11, 2}, kNullId, 5, 11, 16),
      Tile({19, 14}, {6, 4}, 11, 12, 15, 19, false),
      Tile({25, 14}, {5, 4}, 14, 12, kNullId, 19),
      Tile({0, 15}, {19, 3}, kNullId, 13, 14, 19),
      Tile({0, 18}, {7, 4}, kNullId, 16, 18, 20),
      Tile({7, 18}, {6, 4}, 17, 16, 19, 20, false),
      Tile({13, 18}, {17, 4}, 18, 16, kNullId, 20),
      Tile({0, 22}, {30, 2}, kNullId, 17, kNullId, kNullId),
  };
  s.slots_ = {};
  s.last_inserted_ = 11;
  std::array<TestStitch::NeighborGolden, LAST> neighbor_golden;
  neighbor_golden[LEFT] = {
      {0, {}},    {1, {}},        {2, {1, 7}}, {3, {2}},   {4, {3}},
      {5, {}},    {6, {5}},       {7, {6}},    {8, {2}},   {9, {6}},
      {10, {6}},  {11, {10, 13}}, {12, {11}},  {13, {}},   {14, {11, 16}},
      {15, {14}}, {16, {}},       {17, {}},    {18, {17}}, {19, {18}},
      {20, {}},
  };
  neighbor_golden[RIGHT] = {
      {0, {}},    {1, {2}},        {2, {8, 3}}, {3, {4}},   {4, {}},
      {5, {6}},   {6, {10, 9, 7}}, {7, {2}},    {8, {}},    {9, {}},
      {10, {11}}, {11, {14, 12}},  {12, {}},    {13, {11}}, {14, {15}},
      {15, {}},   {16, {14}},      {17, {18}},  {18, {19}}, {19, {}},
      {20, {}},
  };
  neighbor_golden[BOTTOM] = {
      {0, {}},        {1, {0}},           {2, {0}},
      {3, {0}},       {4, {0}},           {5, {1}},
      {6, {1}},       {7, {1}},           {8, {3, 4}},
      {9, {7, 2, 8}}, {10, {9}},          {11, {9}},
      {12, {9}},      {13, {5, 6, 10}},   {14, {12}},
      {15, {12}},     {16, {13, 11}},     {17, {16}},
      {18, {16}},     {19, {16, 14, 15}}, {20, {17, 18, 19}},
  };
  neighbor_golden[TOP] = {
      {0, {4, 3, 2, 1}},  {1, {7, 6, 5}},    {2, {9}},   {3, {8}},
      {4, {8}},           {5, {13}},         {6, {13}},  {7, {9}},
      {8, {9}},           {9, {12, 11, 10}}, {10, {13}}, {11, {16}},
      {12, {15, 14}},     {13, {16}},        {14, {19}}, {15, {19}},
      {16, {19, 18, 17}}, {17, {20}},        {18, {20}}, {19, {20}},
      {20, {}},
  };
  return TestStitch(std::move(s), std::move(neighbor_golden));
}

TEST(PointFinding, Stitch1) {
  auto e = Stitch1();
  e.TestPointFinding();
}

TEST(NeighborFinding, Stitch1) {
  auto e = Stitch1();
  e.TestNeighborFinding();
}

TEST(AreaSearch, Stitch1) {
  auto e = Stitch1();
  e.TestAreaSearch();
  const auto& s = e.s;
  // the whole plane
  EXPECT_EQ(18, s.AreaSearch({s.coord_, s.size_}));
  // custom cases
  EXPECT_EQ(6, s.AreaSearch({{0, 5}, {9, 8}}, 9));
  EXPECT_EQ(11, s.AreaSearch({{0, 2}, {19, 12}}, 4));
  EXPECT_EQ(14, s.AreaSearch({{15, 2}, {15, 16}}, 16));
  EXPECT_EQ(11, s.AreaSearch({{12, 12}, {1, 1}}, 9));
  EXPECT_EQ(2, s.AreaSearch({{9, 2}, {21, 5}}, 20));
  EXPECT_EQ(14, s.AreaSearch({{17, 13}, {13, 5}}));
}

TEST(AreaEnum, Stitch1) {
  auto e = Stitch1();
  e.TestAreaEnum();
  const auto& s = e.s;
  // AreaEnum the whole plane
  EXPECT_EQ((std::vector<Id>{20, 17, 18, 19, 16, 13, 5, 6, 10, 11, 14,
                             15, 12, 9,  7,  1,  2,  8, 3, 4,  0}),
            s.AreaEnum({s.coord_, s.size_}));
  // Custom cases
  EXPECT_EQ((std::vector<Id>{5, 6, 10, 11, 9, 7, 1, 0}),
            s.AreaEnum({s.coord_, s.size_ / 2}));
  EXPECT_EQ((std::vector<Id>{20, 19, 16, 11, 14, 15, 12}),
            s.AreaEnum({s.coord_ + s.size_ / 2, s.size_ / 2}));
  EXPECT_EQ((std::vector<Id>{11, 12, 9, 2, 8, 3, 4, 0}),
            s.AreaEnum({s.coord_ + Pt(s.size_.x / 2, 0), s.size_ / 2}));
  EXPECT_EQ((std::vector<Id>{20, 17, 18, 19, 16, 13, 5, 6, 10, 11}),
            s.AreaEnum({s.coord_ + Pt(0, s.size_.y / 2), s.size_ / 2}));
  EXPECT_EQ((std::vector<Id>{16, 13, 6, 10, 11, 14, 12, 9, 7, 2, 8}),
            s.AreaEnum({s.coord_ + s.size_ / 4, s.size_ / 2}));
}

TEST(VerticalSplitMerge, Stitch1) {
  auto e = Stitch1();
  e.TestVerticalSplitMerge();
}

TEST(HorizontalSplitMerge, Stitch1) {
  auto e = Stitch1();
  e.TestHorizontalSplitMerge();
}

TEST(Insert, Stitch1) {
  auto e = Stitch1();
  e.TestInsert();
  auto& s = e.s;
  std::vector<std::pair<bool, Tile>> cases = {
      {true, {{0, 0}, {1, 24}}},
      {false, {{27, 1}, {2, 4}}},
      {true, {{13, 15}, {6, 7}}},
      {false, {{7, 15}, {6, 6}}},
  };
  for (const auto& [success, t] : cases) {
    if (success) {
      EXPECT_NE(kNullId, s.Insert(t)) << t;
    } else {
      EXPECT_EQ(kNullId, s.Insert(t)) << t;
    }
    e.CheckNeighbors();
    e.CheckStrip();
  }
}