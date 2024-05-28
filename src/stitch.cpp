#include "stitch.hpp"

#include <cassert>

Stitch::Stitch(const Pt& coord, const Pt& size) : coord_(coord), size_(size) {
  assert(coord_.InQuadrantI() && "coord must lies in Quadrant I");
  assert(size_.IsSize() && "illegal size");
}

Id Stitch::PointFinding(const Pt& p, Id start) const {
  Id id = Exist(start) ? start : LastInserted();
  while (id != kNullId && !Ref(id).Contain(p)) {
    // move up/down using rt/lb, until the tile's vertical range contains y
    while (id != kNullId) {
      const auto& t = Ref(id);
      auto cmp_y = t.CmpY(p);
      if (cmp_y == Tile::EQ)
        break;
      else
        id = (cmp_y == Tile::LT) ? t.lb : t.rt;
    }
    // move left/right using tr/bl, until the tile's horizontal range contains x
    while (id != kNullId) {
      const auto& t = Ref(id);
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
  const auto& tl = Ref(id);
  std::vector<Id> neighbors;
  for (Id i = tl.tr;  // 1. go to tr
       Exist(i);      //
       i = Ref(i).lb  // 2. trace down through lb
  ) {
    if (Ref(i).IsRightNeighborTo(tl))
      neighbors.push_back(i);
    else
      break;
  }
  return neighbors;
}

std::vector<Id> Stitch::LeftNeighborFinding(Id id) const {
  assert(Exist(id) && "target tile must exist");
  const auto& tl = Ref(id);
  std::vector<Id> neighbors;
  for (Id i = tl.bl;  // 1. go to bl
       Exist(i);      //
       i = Ref(i).rt  // 2. trace up through rt
  ) {
    if (Ref(i).IsLeftNeighborTo(tl))
      neighbors.push_back(i);
    else
      break;
  }
  return neighbors;
}

std::vector<Id> Stitch::TopNeighborFinding(Id id) const {
  assert(Exist(id) && "target tile must exist");
  const auto& tl = Ref(id);
  std::vector<Id> neighbors;
  for (Id i = tl.rt;  // 1. go to rt
       Exist(i);      //
       i = Ref(i).bl  // 2. trace left through bl
  ) {
    if (Ref(i).IsTopNeighborTo(tl))
      neighbors.push_back(i);
    else
      break;
  }
  return neighbors;
}

std::vector<Id> Stitch::BottomNeighborFinding(Id id) const {
  assert(Exist(id) && "target tile must exist");
  const auto& tl = Ref(id);
  std::vector<Id> neighbors;
  for (Id i = tl.lb;  // 1. go to lb
       Exist(i);      //
       i = Ref(i).tr  // 2. trace right through tr
  ) {
    if (Ref(i).IsBottomNeighborTo(tl))
      neighbors.push_back(i);
    else
      break;
  }
  return neighbors;
}

Id Stitch::AreaSearch(const Tile& area, Id start) const {
  assert(Tile(coord_, size_).Contain(area) &&
         "search area must inside the whole plane");
  // Point-finding the tile containing upper-left corner of the area
  Id id = PointFinding(area.UpperLeftCorner(), start);
  if (!area.Overlap(Ref(id))) {  // look for bottom neighbors
    for (id = Ref(id).lb; Exist(id); id = Ref(id).tr)
      if (area.Overlap(Ref(id))) break;
  }
  while (Exist(id) && Ref(id).is_space) {  // If it is solid tile then return
    // else not solid, check if its right edge overlaps the area
    if (area.OverlapVerticalLine(Ref(id).LowerRightCorner(), Ref(id).size.y)) {
      // then there must be solid tiles in the left neighbors
      for (id = Ref(id).tr; Exist(id); id = Ref(id).lb)
        if (!Ref(id).is_space) return id;
    } else {  // not solid tile found
      // then move down to the next tile overlapping the area
      for (id = Ref(id).lb; Exist(id); id = Ref(id).lb) {
        if (area.Overlap(Ref(id))) break;
      }
    }
  }
  return (Exist(id) && !Ref(id).is_space) ? id : kNullId;
}

Id Stitch::LastInserted() const {
  if (Exist(last_inserted_))
    return last_inserted_;
  else if (NumTiles() > 0)
    for (int i = tiles_.size() - 1; i >= 0; i--)
      if (tiles_[i].has_value()) return i;
  return kNullId;
}
