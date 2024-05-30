#include "test_stitch.hpp"

TestStitch::TestStitch(Stitch&& s,
                       std::array<NeighborGolden, LAST>&& neighbor_golden)
    : s(s), neighbor_golden(neighbor_golden) {
  // check auto generated Neighbors
  for (auto id : Tiles())
    for (int side = 0; side < LAST; side++)
      EXPECT_EQ(neighbor_golden[side][id], Neighbors(id, (Side)side));
  CheckNeighbors();
  CheckTiles();
  CheckStrip();
}

std::vector<Id> TestStitch::Tiles() const {
  std::vector<Id> ids;
  for (size_t i = 0; i < s.tiles_.size(); i++)
    if (s.Exist(i)) ids.push_back(i);
  return ids;
}

std::vector<Id> TestStitch::GoldenNeighbors(Id t, Side side) const {
  EXPECT_NE(LAST, side) << "Invalid side\n";
  if (!s.Exist(t)) return {};
  // collect neighbors linearly
  std::vector<Id> neighbors;
  for (auto id : Tiles()) {
    bool is_neighbor = side == RIGHT  ? s.Ref(id).IsRightNeighborTo(s.Ref(t))
                       : side == LEFT ? s.Ref(id).IsLeftNeighborTo(s.Ref(t))
                       : side == TOP  ? s.Ref(id).IsTopNeighborTo(s.Ref(t))
                                      : s.Ref(id).IsBottomNeighborTo(s.Ref(t));
    if (is_neighbor) neighbors.push_back(id);
  }
  // sort neighbors
  auto cmp = [&](const Id& l, const Id& r) {
    return side == RIGHT  ? s.Ref(l).coord.y > s.Ref(r).coord.y
           : side == LEFT ? s.Ref(l).coord.y < s.Ref(r).coord.y
           : side == TOP  ? s.Ref(l).coord.x > s.Ref(r).coord.x
                          : s.Ref(l).coord.x < s.Ref(r).coord.x;
  };
  std::sort(neighbors.begin(), neighbors.end(), cmp);
  return neighbors;
}

std::vector<Id> TestStitch::Neighbors(Id id, Side side) const {
  EXPECT_NE(LAST, side) << "Invalid side\n";
  return side == RIGHT  ? s.RightNeighborFinding(id)
         : side == LEFT ? s.LeftNeighborFinding(id)
         : side == TOP  ? s.TopNeighborFinding(id)
                        : s.BottomNeighborFinding(id);
}

void TestStitch::CheckNeighbors() const {
  for (auto id : Tiles()) {
    const auto& t = s.Ref(id);
    auto rights = GoldenNeighbors(id, RIGHT);
    EXPECT_EQ(rights, s.RightNeighborFinding(id)) << id;
    EXPECT_EQ(rights.size() ? rights.front() : kNullId, t.tr);
    auto lefts = GoldenNeighbors(id, LEFT);
    EXPECT_EQ(lefts, s.LeftNeighborFinding(id)) << id;
    EXPECT_EQ(lefts.size() ? lefts.front() : kNullId, t.bl);
    auto tops = GoldenNeighbors(id, TOP);
    EXPECT_EQ(tops, s.TopNeighborFinding(id)) << id;
    EXPECT_EQ(tops.size() ? tops.front() : kNullId, t.rt);
    auto bottoms = GoldenNeighbors(id, BOTTOM);
    EXPECT_EQ(bottoms, s.BottomNeighborFinding(id)) << id;
    EXPECT_EQ(bottoms.size() ? bottoms.front() : kNullId, t.lb);
  }
}

void TestStitch::CheckTiles() const {
  std::vector<Id> ids = Tiles();
  Len area = 0;
  for (auto x : ids) {
    area += s.Ref(x).size.x * s.Ref(x).size.y;
    for (auto y : ids) EXPECT_EQ(x == y, s.Ref(x).Overlap(s.Ref(y)));
  }
  EXPECT_FLOAT_EQ(s.size_.x * s.size_.y, area);
}

void TestStitch::CheckStrip() const {
  for (auto id : Tiles()) {
    const auto& tl = s.Ref(id);
    if (tl.is_space) {
      if (s.Exist(tl.bl)) {
        EXPECT_FALSE(s.Ref(tl.bl).is_space) << id;
      }
      if (s.Exist(tl.tr)) {
        EXPECT_FALSE(s.Ref(tl.tr).is_space) << id;
      }
    }
  }
}

void TestStitch::TestPointFinding() const {
  // starting tiles
  std::vector<Id> starts = {
      kNullId,                                      // default
      s.PointFinding(s.coord_),                     // bottom-left
      s.PointFinding(s.coord_ + Pt(s.size_.x, 0)),  // bottom-right
      s.PointFinding(s.coord_ + Pt(0, s.size_.y)),  // top-left
      s.PointFinding(s.coord_ + s.size_)            // top-right
  };
  for (auto it = starts.begin() + 1; it != starts.end(); it++)
    EXPECT_NE(kNullId, *it);
  // for each starting tile, test each tile's all corners
  auto ids = Tiles();
  for (auto start : starts) {
    for (auto id : ids) {
      const auto& t = s.tiles_[id].value();
      EXPECT_EQ(id, s.PointFinding(t.coord, start));
      EXPECT_EQ(id, s.PointFinding(t.coord + Pt(t.size.x - 0.1, 0), start));
      EXPECT_EQ(id, s.PointFinding(t.coord + Pt(0, t.size.y - 0.1), start));
      EXPECT_EQ(id, s.PointFinding(t.coord + t.size + Pt(-0.1, -0.1), start));
      EXPECT_EQ(id, s.PointFinding(t.coord + t.size / 2));
      if (s.Ref(id).rt == kNullId && s.Ref(id).tr == kNullId) {
        EXPECT_EQ(id, s.PointFinding(t.coord + Pt(t.size.x, 0), start));
        EXPECT_EQ(id, s.PointFinding(t.coord + Pt(0, t.size.y), start));
        EXPECT_EQ(id, s.PointFinding(t.coord + t.size, start));
      } else if (s.Ref(id).tr == kNullId) {
        EXPECT_EQ(id, s.PointFinding(t.coord + Pt(t.size.x, 0), start));
        EXPECT_NE(id, s.PointFinding(t.coord + Pt(0, t.size.y), start));
        EXPECT_NE(id, s.PointFinding(t.coord + t.size, start));
      } else if (s.Ref(id).rt == kNullId) {
        EXPECT_NE(id, s.PointFinding(t.coord + Pt(t.size.x, 0), start));
        EXPECT_EQ(id, s.PointFinding(t.coord + Pt(0, t.size.y), start));
        EXPECT_NE(id, s.PointFinding(t.coord + t.size, start));
      } else {
        EXPECT_NE(id, s.PointFinding(t.coord + Pt(t.size.x, 0), start));
        EXPECT_NE(id, s.PointFinding(t.coord + Pt(0, t.size.y), start));
        EXPECT_NE(id, s.PointFinding(t.coord + t.size, start));
      }
    }
  }
}

void TestStitch::TestNeighborFinding() const {
  for (auto id : Tiles())
    for (int side = 0; side != LAST; side++)
      EXPECT_EQ(GoldenNeighbors(id, (Side)side), Neighbors(id, (Side)side));
}

void TestStitch::TestAreaSearch() const {
  // AreaSearch self should return self if solid
  for (auto id : Tiles()) {
    Tile t = s.At(id).value();
    EXPECT_EQ(t.is_space ? kNullId : id, s.AreaSearch(t));
  }
}

void TestStitch::TestAreaEnum() const {
  // AreaEnum self should return {self}
  for (auto id : Tiles()) EXPECT_EQ(std::vector<Id>{id}, s.AreaEnum(s.Ref(id)));
}

bool StitchEqual(const Stitch& l, const Stitch& r) {
  if (l.NumTiles() != r.NumTiles()) return false;
  // collect tiles
  std::vector<Id> l_ids;
  for (size_t i = 0; i < l.tiles_.size(); i++)
    if (l.tiles_[i].has_value()) l_ids.push_back(i);
  std::vector<Id> r_ids;
  for (size_t i = 0; i < r.tiles_.size(); i++)
    if (r.tiles_[i].has_value()) r_ids.push_back(i);

  if (l_ids != r_ids) return false;
  for (auto id : l_ids) {
    if (l.Ref(id) != r.Ref(id)) return false;
  }
  return true;
}

Stitch TestStitch::TestVerticalSplitMerge() const {
  auto ns = s;
  auto ids = Tiles();
  // split
  std::vector<Id> nids;
  for (auto id : ids) {
    size_t size = ns.NumTiles();
    const auto& t = ns.Ref(id);
    auto nid = ns.VerticalSplit(id, t.coord.x + t.size.x / 2);
    EXPECT_NE(nid, id) << "should return id of new tile";
    EXPECT_EQ(size + 1, ns.NumTiles());
    CheckNeighbors();
    CheckTiles();
    nids.push_back(nid);
  }
  EXPECT_FALSE(StitchEqual(ns, s));
  // merge
  for (size_t i = 0; i < ids.size(); i++) {
    size_t size = ns.NumTiles();
    Id id = ids[i], nid = nids[i];
    auto mrg = ns.VerticalMerge(id, nid);
    EXPECT_EQ(id, mrg);
    EXPECT_EQ(size, ns.NumTiles() + 1);
    CheckNeighbors();
    CheckTiles();
  }
  CheckStrip();
  EXPECT_TRUE(StitchEqual(ns, s));
  return ns;
}

Stitch TestStitch::TestHorizontalSplitMerge() const {
  auto ns = s;
  auto ids = Tiles();
  // split
  std::vector<Id> nids;
  for (auto id : ids) {
    size_t size = ns.NumTiles();
    const auto& t = ns.Ref(id);
    auto nid = ns.HorizontalSplit(id, t.coord.y + t.size.y / 2);
    EXPECT_NE(nid, id) << "should return id of new tile";
    EXPECT_EQ(size + 1, ns.NumTiles());
    CheckNeighbors();
    CheckTiles();
    nids.push_back(nid);
  }
  EXPECT_FALSE(StitchEqual(ns, s));
  // merge
  for (size_t i = 0; i < ids.size(); i++) {
    size_t size = ns.NumTiles();
    Id id = ids[i], nid = nids[i];
    auto mrg = ns.HorizontalMerge(id, nid);
    EXPECT_EQ(id, mrg);
    EXPECT_EQ(size, ns.NumTiles() + 1);
    CheckNeighbors();
    CheckTiles();
  }
  CheckStrip();
  EXPECT_TRUE(StitchEqual(ns, s));
  return ns;
}

Stitch TestStitch::TestInsert() const {
  auto ns = s;
  for (auto id : Tiles()) {
    size_t size = ns.NumTiles();
    if (ns.Ref(id).is_space) {
      EXPECT_NE(kNullId, ns.Insert(ns.Ref(id))) << id;
    } else {
      EXPECT_EQ(kNullId, ns.Insert(ns.Ref(id))) << id;
    }
    EXPECT_EQ(size, ns.NumTiles());
    CheckNeighbors();
    CheckTiles();
    CheckStrip();
    EXPECT_FALSE(StitchEqual(ns, s));
  }
  return ns;
}

Stitch TestStitch::TestInsert(
    const std::vector<std::tuple<bool, Tile>>& cases) const {
  auto ns = s;
  for (const auto& [success, t] : cases) {
    Id nid = ns.Insert(t);
    if (success) {
      EXPECT_NE(kNullId, nid) << t;
      EXPECT_EQ(ns.Ref(nid).coord, t.coord);
      EXPECT_EQ(ns.Ref(nid).size, t.size);
    } else {
      EXPECT_EQ(kNullId, nid) << t;
    }
    CheckNeighbors();
    CheckTiles();
    CheckStrip();
  }
  return ns;
}
