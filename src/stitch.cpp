#include "stitch.hpp"

#include <cassert>

Stitch::Stitch(const Pt& coord, const Pt& size) : coord_(coord), size_(size) {
  assert(coord_.InQuadrantI() && "coord must lies in Quadrant I");
  assert(size_.IsSize() && "illegal size");
  assert(size_.x > 0 && size_.y > 0 && "plane without area is no supported");
}

Id Stitch::PointFinding(const Pt& p, Id start) const {
  Id id = Exist(start) ? start : LastInserted();
  Id prev_id = kNullId;
  while (id != kNullId && !Ref(id).Contain(p) && id != prev_id) {
    prev_id = id;
    // move up/down using rt/lb, until the tile's vertical range contains y
    while (id != kNullId) {
      const auto& t = Ref(id);
      auto cmp_y = t.CmpY(p);
      if (cmp_y == Tile::EQ)
        break;
      else if (cmp_y == Tile::GT && t.rt == kNullId)
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
      else if (cmp_x == Tile::GT && t.tr == kNullId)
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
      for (id = Ref(id).lb; Exist(id); id = Ref(id).tr) {
        if (area.Overlap(Ref(id))) break;
      }
    }
  }
  return (Exist(id) && !Ref(id).is_space) ? id : kNullId;
}

std::vector<Id> Stitch::AreaEnum(const Tile& area, Id start) const {
  assert(Tile(coord_, size_).Contain(area) &&
         "search area must inside the whole plane");
  std::vector<Id> enums;
  // Point-finding the tile containing upper-left corner of the area
  Id id = PointFinding(area.UpperLeftCorner(), start);
  if (!area.Overlap(Ref(id))) {  // look for bottom neighbors
    for (id = Ref(id).lb; Exist(id); id = Ref(id).tr)
      if (area.Overlap(Ref(id))) break;
  }
  // Step down through all the tiles along the left edge, as in AreaSearch
  while (Exist(id) &&
         Ref(id).OverlapVerticalLine(area.LowerLeftCorner(), area.size.y)) {
    AreaEnumHelper(area, enums, id);  // calling helper (R procedure)
    // move down to the next tile overlapping the area
    for (id = Ref(id).lb; Exist(id); id = Ref(id).tr) {
      if (Ref(id).OverlapVerticalLine(area.LowerLeftCorner(), area.size.y))
        break;
    }
  }
  return enums;
}

void Stitch::AreaEnumHelper(const Tile& area, std::vector<Id>& enums,
                            Id id) const {
  // 1. Enumerate the tile
  enums.push_back(id);
  // 2. If the right edge of the tile is outside of the search area, return
  if (!area.OverlapVerticalLine(Ref(id).LowerRightCorner(), Ref(id).size.y))
    return;
  // 3. Otherwise, use the neighbor-finding algorithm to locate all the tiles
  // that touch the right side of the current tile and also intersect the
  // searching area.
  for (const auto& i : RightNeighborFinding(id)) {
    if (area.Overlap(Ref(i))) {
      // 4. For each of these neighbors, if the bottom left corner of the
      // neighbor touches the current tile then call R to enumerate the
      // neighbor recursively.
      const auto& llc = Ref(i).LowerLeftCorner();
      if (Ref(id).LowerRightCorner().x == llc.x &&
          Ref(id).CmpY(llc.y) == Tile::EQ) {
        AreaEnumHelper(area, enums, i);
      }
      // 5. Or, if the bottom edge of the search area cuts both the current
      // tile and the neighbor, then call R to enumerate the neighbor
      // recursively.
      else if (  //
          Ref(id).OverlapHorizontalLine(area.LowerLeftCorner(), area.size.x) &&
          Ref(i).OverlapHorizontalLine(area.LowerLeftCorner(), area.size.x)) {
        AreaEnumHelper(area, enums, i);
      }
    }
  }
}

Id Stitch::AllocTile() {
  Id id = kNullId;
  if (slots_.size()) {
    id = slots_.top();
    slots_.pop();
    tiles_[id] = Tile();
  } else {
    id = tiles_.size();
    tiles_.push_back(Tile());
  }
  return id;
}

int Stitch::FreeTile(Id id) {
  if (id != kNullId && 0 <= id && (size_t)id < tiles_.size()) {
    tiles_[id] = std::nullopt;
    slots_.push(id);
    return 0;
  } else {
    return 1;
  }
}

Id Stitch::VerticalSplit(Id left, Len x) {
  if (!Exist(left) || Ref(left).CmpX(x) != Tile::EQ ||
      Ref(left).coord.x == x)  // cannot split on edge
    return kNullId;
  // create a copy (left is the original tile)
  Id right = AllocTile();
  Ref(right) = Ref(left);
  // collect neighbors of the original tile
  auto right_neighbors = RightNeighborFinding(left);
  auto bottom_neighbors = BottomNeighborFinding(left);
  auto top_neighbors = TopNeighborFinding(left);
  // update coord & size of the original & new tiles
  auto orig_size_x = Ref(left).size.x;
  Ref(left).size.x = x - Ref(left).coord.x;
  Ref(right).coord.x = x;
  Ref(right).size.x -= Ref(left).size.x;
  assert(orig_size_x == (Ref(left).size.x + Ref(right).size.x));
  // adjust the stitch between original & new tiles
  Ref(right).bl = left;
  Ref(left).tr = right;
  Ref(right).lb = Ref(left).rt = kNullId;  // to be determined
  // Update the stitches in tiles that are now adj to the new tile
  // 1. for right, if bl = orig, change to new
  for (auto id : right_neighbors)
    if (Ref(id).bl == left) Ref(id).bl = right;
  // 2. for lower, if rt = orig, change to new if it touches new
  for (auto id : bottom_neighbors) {
    if (Ref(id).rt == left) {
      if (Ref(right).IsTopNeighborTo(Ref(id))) {
        Ref(id).rt = right;
        // the first satisfying below must be the right-most
        if (Ref(right).lb == kNullId) Ref(right).lb = id;
      }
    }
  }
  // 3. for top, if lb = orig, change to new if it does not touch orig
  for (auto id : top_neighbors) {
    if (Ref(id).lb == left) {
      Ref(id).lb = right;  // flip
      if (Ref(left).IsBottomNeighborTo(Ref(id))) {
        Ref(id).lb = left;
        // the first satisfying below must be the right-most
        if (Ref(left).rt == kNullId) Ref(left).rt = id;
      }
    }
  }
  return right;
}

Id Stitch::HorizontalSplit(Id lower, Len y) {
  if (!Exist(lower) || Ref(lower).CmpY(y) != Tile::EQ ||
      Ref(lower).coord.y == y)  // cannot split on edge
    return kNullId;
  // create a copy (lower is the original tile)
  Id upper = AllocTile();  // (upper is the new tile)
  Ref(upper) = Ref(lower);
  // collect neighbors of the original tile
  auto top_neighbors = TopNeighborFinding(lower);
  auto left_neighbors = LeftNeighborFinding(lower);
  auto right_neighbors = RightNeighborFinding(lower);
  // update coord & size of original & new tiles
  auto orig_size_y = Ref(lower).size.y;
  Ref(lower).size.y = y - Ref(lower).coord.y;
  Ref(upper).coord.y = y;
  Ref(upper).size.y -= Ref(lower).size.y;
  assert(orig_size_y == (Ref(lower).size.y + Ref(upper).size.y));
  // adjust the stitch between original & new tiles
  Ref(upper).lb = lower;
  Ref(lower).rt = upper;
  Ref(upper).bl = Ref(lower).tr = kNullId;  // to be determined
  // Update the stitches in tiles that are now adj to the new tile
  // 1. for upper, if lb = orig, change to new
  for (auto id : top_neighbors)
    if (Ref(id).lb == lower) Ref(id).lb = upper;
  // 2. for left, if tr = orig, change to new if it touches new
  for (auto id : left_neighbors) {
    if (Ref(id).tr == lower) {
      if (Ref(upper).IsRightNeighborTo(Ref(id))) {
        Ref(id).tr = upper;
        // the first satisfying below must be the lowest
        if (Ref(upper).bl == kNullId) Ref(upper).bl = id;
      }
    }
  }
  // 3. for right, if bl = orig, change to to new if it does not touch orig
  for (auto id : right_neighbors) {
    if (Ref(id).bl == lower) {
      Ref(id).bl = upper;  // flip
      if (Ref(lower).IsLeftNeighborTo(Ref(id))) {
        Ref(id).bl = lower;
        // the first satisfying below must be the highest
        if (Ref(lower).tr == kNullId) Ref(lower).tr = id;
      }
    }
  }
  return upper;
}

Id Stitch::LastInserted() const {
  if (Exist(last_inserted_))
    return last_inserted_;
  else if (NumTiles() > 0)
    for (int i = tiles_.size() - 1; i >= 0; i--)
      if (tiles_[i].has_value()) return i;
  return kNullId;
}
