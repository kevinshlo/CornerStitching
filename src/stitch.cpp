#include "stitch.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

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
  if (!Exist(id)) return {};
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
  if (!Exist(id)) return {};
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
  if (!Exist(id)) return {};
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
  if (!Exist(id)) return {};
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
  if (!area.coord.InQuadrantI() || !area.size.IsSize() ||
      !Tile(coord_, size_).Contain(area))
    return kNullId;
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
  if (!area.coord.InQuadrantI() || !area.size.IsSize() ||
      !Tile(coord_, size_).Contain(area))
    return {};
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

Id Stitch::Insert(Tile tile) {
  if (!Tile(coord_, size_).Contain(tile)) return kNullId;
  // if there's solid tiles in the inserted tile, abort
  if (AreaSearch(tile) != kNullId) return kNullId;
  // 1. Find the space tile containing the top edge of the new tile
  Id top = PointFinding(tile.UpperLeftCorner());
  if (!tile.Overlap(Ref(top)))  // look for bottom neighbors
    for (top = Ref(top).lb; Exist(top); top = Ref(top).tr)
      if (tile.Overlap(Ref(top))) break;
  // 2. split the top tile
  HorizontalSplit(top, tile.UpperLeftCorner().y);
  // 3. Find & split the space tile containing the bottom edge of the new tile
  Id bottom = PointFinding(tile.LowerLeftCorner(), top);
  Id new_bottom = HorizontalSplit(bottom, tile.LowerLeftCorner().y);
  if (new_bottom != kNullId) bottom = new_bottom;
  // 4. wall down along left side as AreaSearch
  Id last_id = kNullId, left = kNullId, right = kNullId;
  for (Id id = top; Exist(id) && tile.Overlap(Ref(id));) {
    // split left
    left = VerticalSplit(id, tile.LowerLeftCorner().x);
    if (left != kNullId) std::swap(left, id);
    HorizontalMerge(left, Ref(left).rt);
    // split right
    right = VerticalSplit(id, tile.LowerRightCorner().x);
    HorizontalMerge(right, Ref(right).rt);
    // merge middle
    HorizontalMerge(id, last_id);
    last_id = id;
    // move down
    for (id = Ref(id).lb; Exist(id); id = Ref(id).tr) {
      if (tile.Overlap(Ref(id))) break;
      if (tile.CmpX(Ref(id).coord.x) == Tile::GT) id = kNullId;
    }
  }
  HorizontalMerge(Ref(left).lb, left);
  HorizontalMerge(Ref(right).lb, right);
  Ref(last_id).is_space = false;
  return last_id;
}

std::optional<Tile> Stitch::Delete(Id dead) {
  if (!Exist(dead) || Ref(dead).is_space) return std::nullopt;
  auto ret = Ref(dead);
  // change the type of the dead tile to space.
  Ref(dead).is_space = true;
  auto top = Ref(dead).UpperRightCorner().y;
  auto bottom = Ref(dead).LowerLeftCorner().y;
  // collect all right & left neighbors
  auto right_neighbors = RightNeighborFinding(dead);
  auto left_neighbors = LeftNeighborFinding(dead);
  // split & merge right neighbors
  auto cut = top;
  std::vector<Id> split_dead;  // from top to bottom
  for (auto r : right_neighbors) {
    if (top < Ref(r).UpperLeftCorner().y && Ref(r).is_space) {
      // space neighbor containing top of dead tile should be split
      // this should only happens when visiting the first neighbor
      HorizontalSplit(r, cut);
    }
    cut = Ref(r).LowerLeftCorner().y;  // move down cut edge
    if (cut > bottom) {                // dead tile need to be cut
      auto dead_upper = HorizontalSplit(dead, cut);
      VerticalMerge(dead_upper, r);  // solid r must filed
      split_dead.push_back(dead_upper);
    } else if (cut < bottom) {                 // neighbor need to be cut
      auto r_upper = HorizontalSplit(r, cut);  // solid r must filed
      VerticalMerge(dead, r_upper);            // solid r must filed
      split_dead.push_back(dead);
    } else {                   // no need to cut
      VerticalMerge(dead, r);  // solid r must filed
      split_dead.push_back(dead);
    }
  }
  // split & merge left neighbors
  std::vector<Id> merge_dead;
  for (auto l = left_neighbors.front();
       Exist(l) && Ref(l).IsLeftNeighborTo(ret) && split_dead.size();) {
    if (Ref(l).is_space) {
      auto l_rt = Ref(l).rt;
      auto d = split_dead.back();
      split_dead.pop_back();
      if (Ref(l).LowerRightCorner().y > Ref(d).LowerLeftCorner().y) {
        // cut the lower part of d which does not touch l
        // this should only happens when visiting the first neighbor
        auto d_upper = HorizontalSplit(d, Ref(l).LowerRightCorner().y);
        merge_dead.push_back(d);  // d_lower
        d = d_upper;
      }
      if (Ref(l).UpperRightCorner().y > Ref(d).UpperLeftCorner().y) {
        // cut the lower part of l which touches d, merge
        auto l_upper = HorizontalSplit(l, Ref(d).UpperLeftCorner().y);
        VerticalMerge(l, d);
        merge_dead.push_back(l);  // l_lower & d
        l = l_upper;
      } else if (Ref(l).UpperRightCorner().y < Ref(d).UpperLeftCorner().y) {
        // cut the lower part of d which touches l, merge
        auto d_upper = HorizontalSplit(d, Ref(l).UpperRightCorner().y);
        split_dead.push_back(d_upper);  // handle in next iter
        VerticalMerge(l, d);
        merge_dead.push_back(l);  // l & d_lower
        l = l_rt;
      } else {  // l & d have same vertical span, merge
        VerticalMerge(l, d);
        merge_dead.push_back(l);
        l = l_rt;
      }
    } else {
      // no need to split d if l is solid
      while (split_dead.size() &&
             (Ref(l).UpperRightCorner().y >=
              Ref(split_dead.back()).UpperLeftCorner().y)) {
        merge_dead.push_back(split_dead.back());
        split_dead.pop_back();
      }
      l = Ref(l).rt;
    }
  }
  // merge space tiles if possible
  for (auto d : merge_dead) {
    if (Exist(d)) {
      HorizontalMerge(d, Ref(d).rt);
      HorizontalMerge(Ref(d).lb, d);
    }
  }
  return std::optional<Tile>{ret};
}

Id Stitch::LastInserted() const {
  if (Exist(last_inserted_))
    return last_inserted_;
  else if (NumTiles() > 0)
    for (int i = tiles_.size() - 1; i >= 0; i--)
      if (tiles_[i].has_value()) return i;
  return kNullId;
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
    if (Ref(id).rt == left && Ref(right).IsTopNeighborTo(Ref(id)))
      Ref(id).rt = right;
    if (Ref(right).lb == kNullId && Ref(right).IsTopNeighborTo(Ref(id)))
      Ref(right).lb = id;  // must be the left-most
  }
  // 3. for top, if lb = orig, change to new if it does not touch orig
  for (auto id : top_neighbors) {
    if (Ref(id).lb == left && !Ref(left).IsBottomNeighborTo(Ref(id)))
      Ref(id).lb = right;
    if (Ref(left).rt == kNullId && Ref(left).IsBottomNeighborTo(Ref(id)))
      Ref(left).rt = id;  // must be the right-most
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
    if (Ref(id).tr == lower && Ref(upper).IsRightNeighborTo(Ref(id)))
      Ref(id).tr = upper;
    if (Ref(upper).bl == kNullId && Ref(upper).IsRightNeighborTo(Ref(id)))
      Ref(upper).bl = id;  // must be the lowest
  }
  // 3. for right, if bl = orig, change to to new if it does not touch orig
  for (auto id : right_neighbors) {
    if (Ref(id).bl == lower && !Ref(lower).IsLeftNeighborTo(Ref(id)))
      Ref(id).bl = upper;
    if (Ref(lower).tr == kNullId && Ref(lower).IsLeftNeighborTo(Ref(id)))
      Ref(lower).tr = id;  // must be the highest
  }
  return upper;
}

Id Stitch::VerticalMerge(Id left, Id right) {
  if (!Exist(left) || !Exist(right)) return kNullId;
  if (Ref(left).is_space != Ref(right).is_space)
    return kNullId;  // must be same type
  if (Ref(left).coord.y != Ref(right).coord.y ||
      Ref(left).size.y != Ref(right).size.y)  // must align
    return kNullId;
  if (Ref(left).coord.x > Ref(right).coord.x) std::swap(left, right);
  // update stitches touching new (right)
  // 1. for right, if bl = new, change to orig
  for (auto id : RightNeighborFinding(right))
    if (Ref(id).bl == right) Ref(id).bl = left;
  // 2. for lower, if rt = new, change to orig
  for (auto id : BottomNeighborFinding(right))
    if (Ref(id).rt == right) Ref(id).rt = left;
  // 3. for top, if lb = new, change to orig
  for (auto id : TopNeighborFinding(right))
    if (Ref(id).lb == right) Ref(id).lb = left;
  // adjust the stitch of original tile
  Ref(left).tr = Ref(right).tr;
  Ref(left).rt = Ref(right).rt;
  // update coord & size of original tile
  Ref(left).size.x = Ref(right).coord.x + Ref(right).size.x - Ref(left).coord.x;
  // free the new tile
  FreeTile(right);
  return left;
}

Id Stitch::HorizontalMerge(Id lower, Id upper) {
  if (!Exist(lower) || !Exist(upper)) return kNullId;
  if (Ref(lower).is_space != Ref(upper).is_space)
    return kNullId;  // must be same type
  if (Ref(lower).coord.x != Ref(upper).coord.x ||
      Ref(lower).size.x != Ref(upper).size.x)  // must align
    return kNullId;
  if (Ref(lower).coord.y > Ref(upper).coord.y) std::swap(lower, upper);
  // update stitches touching new (upper)
  // 1. for upper, if lb = new, change to orig
  for (auto id : TopNeighborFinding(upper))
    if (Ref(id).lb == upper) Ref(id).lb = lower;
  // 2. for left, if tr = new, change to orig
  for (auto id : LeftNeighborFinding(upper))
    if (Ref(id).tr == upper) Ref(id).tr = lower;
  // 3. for right, if bl = new, change to orig
  for (auto id : RightNeighborFinding(upper))
    if (Ref(id).bl == upper) Ref(id).bl = lower;
  // adjust the stitch of original tile
  Ref(lower).tr = Ref(upper).tr;
  Ref(lower).rt = Ref(upper).rt;
  // update coord & size of original tile
  Ref(lower).size.y =
      Ref(upper).coord.y + Ref(upper).size.y - Ref(lower).coord.y;
  // free the new tile
  FreeTile(upper);
  return lower;
}
