#include "stitch.hpp"

#include <cassert>

Stitch::Stitch(const Pt& coord, const Pt& size) : coord_(coord), size_(size) {
  assert(coord_.InQuadrantI() && "coord must lies in Quadrant I");
  assert(size_.IsSize() && "illegal size");
}

Id Stitch::PointFinding(const Pt& p, Id start) const {
  Id id = Exist(start) ? start : LastInserted();
  while (id != kNullId && !tiles_[id].value().Contain(p)) {
    // move up/down using rt/lb, until the tile's vertical range contains y
    while (id != kNullId) {
      assert(tiles_[id].has_value() && "id must point to a real tile");
      const auto& t = tiles_[id].value();
      auto cmp_y = t.CmpY(p);
      if (cmp_y == Tile::EQ)
        break;
      else
        id = (cmp_y == Tile::LT) ? t.lb : t.rt;
    }
    // move left/right using tr/bl, until the tile's horizontal range contains x
    while (id != kNullId) {
      assert(tiles_[id].has_value() && "id must point to a real tile");
      const auto& t = tiles_[id].value();
      auto cmp_x = t.CmpX(p);
      if (cmp_x == Tile::EQ)
        break;
      else
        id = (cmp_x == Tile::LT) ? t.bl : t.tr;
    }
    // repeat since misalignment might occur
  }
  return id;
}

std::vector<Id> Stitch::RightNeighborFinding(Id id) const {
  assert(Exist(id) && "target tile must exist");
  const auto& tl = tiles_[id].value();
  std::vector<Id> neighbors;
  for (Id i = tl.tr;             // 1. go to tr
       Exist(i);                 //
       i = tiles_[i].value().lb  // 2. trace down through lb
  ) {
    if (tiles_[i].value().IsRightNeighborTo(tl))
      neighbors.push_back(i);
    else
      break;
  }
  return neighbors;
}

std::vector<Id> Stitch::LeftNeighborFinding(Id id) const {
  assert(Exist(id) && "target tile must exist");
  const auto& tl = tiles_[id].value();
  std::vector<Id> neighbors;
  for (Id i = tl.bl;             // 1. go to bl
       Exist(i);                 //
       i = tiles_[i].value().rt  // 2. trace up through rt
  ) {
    if (tiles_[i].value().IsLeftNeighborTo(tl))
      neighbors.push_back(i);
    else
      break;
  }
  return neighbors;
}

std::vector<Id> Stitch::TopNeighborFinding(Id id) const {
  assert(Exist(id) && "target tile must exist");
  const auto& tl = tiles_[id].value();
  std::vector<Id> neighbors;
  for (Id i = tl.rt;             // 1. go to rt
       Exist(i);                 //
       i = tiles_[i].value().bl  // 2. trace left through bl
  ) {
    if (tiles_[i].value().IsTopNeighborTo(tl))
      neighbors.push_back(i);
    else
      break;
  }
  return neighbors;
}

std::vector<Id> Stitch::BottomNeighborFinding(Id id) const {
  assert(Exist(id) && "target tile must exist");
  const auto& tl = tiles_[id].value();
  std::vector<Id> neighbors;
  for (Id i = tl.lb;             // 1. go to lb
       Exist(i);                 //
       i = tiles_[i].value().tr  // 2. trace right through tr
  ) {
    if (tiles_[i].value().IsBottomNeighborTo(tl))
      neighbors.push_back(i);
    else
      break;
  }
  return neighbors;
}

Id Stitch::LastInserted() const {
  if (Exist(last_inserted_))
    return last_inserted_;
  else if (Size())
    for (int i = tiles_.size() - 1; i >= 0; i--)
      if (tiles_[i].has_value()) return i;
  return kNullId;
}
