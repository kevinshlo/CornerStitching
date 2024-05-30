#include "test_stitch.hpp"

TestStitch::TestStitch(Stitch&& s,
                       std::array<NeighborGolden, LAST>&& neighbor_golden)
    : s(s), neighbor_golden(neighbor_golden) {
  // check auto generated Neighbors
  for (auto id : Tiles(s))
    for (int side = 0; side < LAST; side++)
      EXPECT_EQ(neighbor_golden[side][id], Neighbors(s, id, (Side)side));
  CheckNeighbors(s);
  CheckTiles(s);
  CheckStrip(s);
}

std::vector<Id> TestStitch::Tiles(const Stitch& stitch) const {
  std::vector<Id> ids;
  for (size_t i = 0; i < stitch.tiles_.size(); i++)
    if (stitch.Exist(i)) ids.push_back(i);
  return ids;
}

std::vector<Id> TestStitch::GoldenNeighbors(const Stitch& s, Id t,
                                            Side side) const {
  EXPECT_NE(LAST, side) << "Invalid side\n";
  if (!s.Exist(t)) return {};
  // collect neighbors linearly
  std::vector<Id> neighbors;
  for (auto id : Tiles(s)) {
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

std::vector<Id> TestStitch::Neighbors(const Stitch& s, Id id, Side side) const {
  EXPECT_NE(LAST, side) << "Invalid side\n";
  return side == RIGHT  ? s.RightNeighborFinding(id)
         : side == LEFT ? s.LeftNeighborFinding(id)
         : side == TOP  ? s.TopNeighborFinding(id)
                        : s.BottomNeighborFinding(id);
}

void TestStitch::CheckNeighbors(const Stitch& s) const {
  for (auto id : Tiles(s)) {
    const auto& t = s.Ref(id);
    auto rights = GoldenNeighbors(s, id, RIGHT);
    EXPECT_EQ(rights, s.RightNeighborFinding(id)) << id;
    EXPECT_EQ(rights.size() ? rights.front() : kNullId, t.tr);
    auto lefts = GoldenNeighbors(s, id, LEFT);
    EXPECT_EQ(lefts, s.LeftNeighborFinding(id)) << id;
    EXPECT_EQ(lefts.size() ? lefts.front() : kNullId, t.bl);
    auto tops = GoldenNeighbors(s, id, TOP);
    EXPECT_EQ(tops, s.TopNeighborFinding(id)) << id;
    EXPECT_EQ(tops.size() ? tops.front() : kNullId, t.rt);
    auto bottoms = GoldenNeighbors(s, id, BOTTOM);
    EXPECT_EQ(bottoms, s.BottomNeighborFinding(id)) << id;
    EXPECT_EQ(bottoms.size() ? bottoms.front() : kNullId, t.lb);
  }
}

void TestStitch::CheckTiles(const Stitch& s) const {
  std::vector<Id> ids = Tiles(s);
  Len area = 0;
  for (auto x : ids) {
    area += s.Ref(x).size.x * s.Ref(x).size.y;
    for (auto y : ids) EXPECT_EQ(x == y, s.Ref(x).Overlap(s.Ref(y)));
  }
  EXPECT_FLOAT_EQ(s.size_.x * s.size_.y, area);
}

void TestStitch::CheckStrip(const Stitch& s) const {
  for (auto id : Tiles(s)) {
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
  auto ids = Tiles(s);
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
  for (auto id : Tiles(s))
    for (int side = 0; side != LAST; side++)
      EXPECT_EQ(GoldenNeighbors(s, id, (Side)side),
                Neighbors(s, id, (Side)side));
}

void TestStitch::TestAreaSearch() const {
  // AreaSearch self should return self if solid
  for (auto id : Tiles(s)) {
    Tile t = s.At(id).value();
    EXPECT_EQ(t.is_space ? kNullId : id, s.AreaSearch(t));
  }
}

void TestStitch::TestAreaEnum() const {
  // AreaEnum self should return {self}
  for (auto id : Tiles(s))
    EXPECT_EQ(std::vector<Id>{id}, s.AreaEnum(s.Ref(id)));
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
  auto ids = Tiles(ns);
  // split
  std::vector<Id> nids;
  for (auto id : ids) {
    std::cout << "vertical split: " << id << std::endl;
    size_t size = ns.NumTiles();
    const auto& t = ns.Ref(id);
    auto nid = ns.VerticalSplit(id, t.coord.x + t.size.x / 2);
    EXPECT_NE(nid, id) << "should return id of new tile";
    EXPECT_EQ(size + 1, ns.NumTiles());
    CheckNeighbors(ns);
    CheckTiles(ns);
    nids.push_back(nid);
  }
  EXPECT_FALSE(StitchEqual(ns, s));
  // merge
  for (size_t i = 0; i < ids.size(); i++) {
    size_t size = ns.NumTiles();
    Id id = ids[i], nid = nids[i];
    std::cout << "vertical merge: " << id << " " << nid << std::endl;
    auto mrg = ns.VerticalMerge(id, nid);
    EXPECT_EQ(id, mrg);
    EXPECT_EQ(size, ns.NumTiles() + 1);
    CheckNeighbors(ns);
    CheckTiles(ns);
  }
  CheckStrip(ns);
  EXPECT_TRUE(StitchEqual(ns, s));
  return ns;
}

Stitch TestStitch::TestHorizontalSplitMerge() const {
  auto ns = s;
  auto ids = Tiles(ns);
  // split
  std::vector<Id> nids;
  for (auto id : ids) {
    size_t size = ns.NumTiles();
    const auto& t = ns.Ref(id);
    auto nid = ns.HorizontalSplit(id, t.coord.y + t.size.y / 2);
    EXPECT_NE(nid, id) << "should return id of new tile";
    EXPECT_EQ(size + 1, ns.NumTiles());
    CheckNeighbors(ns);
    CheckTiles(ns);
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
    CheckNeighbors(ns);
    CheckTiles(ns);
  }
  CheckStrip(ns);
  EXPECT_TRUE(StitchEqual(ns, s));
  return ns;
}

Stitch TestStitch::TestInsert() const {
  auto ns = s;
  for (auto id : Tiles(ns)) {
    size_t size = ns.NumTiles();
    if (ns.Ref(id).is_space) {
      EXPECT_NE(kNullId, ns.Insert(ns.Ref(id))) << id;
    } else {
      EXPECT_EQ(kNullId, ns.Insert(ns.Ref(id))) << id;
    }
    EXPECT_EQ(size, ns.NumTiles());
    CheckNeighbors(ns);
    CheckTiles(ns);
    CheckStrip(ns);
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
    CheckNeighbors(ns);
    CheckTiles(ns);
    CheckStrip(ns);
  }
  return ns;
}

Stitch TestStitch::TestDelete() const {
  auto ns = s;
  for (auto id : Tiles(ns)) {
    auto t = ns.At(id).value();
    auto opt = ns.Delete(id);
    if (ns.Ref(id).is_space) {
      EXPECT_EQ(std::nullopt, opt);
    } else {
      EXPECT_EQ(t, opt.value());
    }
    CheckNeighbors(ns);
    CheckTiles(ns);
    CheckStrip(ns);
  }
  return ns;
}
