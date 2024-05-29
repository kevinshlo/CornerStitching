#pragma once

#include <optional>
#include <stack>
#include <vector>

#include "tile.hpp"

class Stitch {
 public:
  Stitch() = default;
  Stitch(const Stitch& stitch) = default;
  Stitch(const Pt& coord, const Pt& size);

  /* number of tiles */
  size_t NumTiles() const { return tiles_.size() - slots_.size(); }
  /* get a copy of tile */
  std::optional<Tile> At(Id id) const {
    return (id != kNullId && 0 <= id && (size_t)id < tiles_.size())
               ? tiles_[id]
               : std::nullopt;
  }
  // find the tile at `pt`, default start at `last_inserted_`
  Id PointFinding(const Pt& pt, Id start = kNullId) const;
  // find all neighbors contacting the right edge of tile `id` (top to down)
  std::vector<Id> RightNeighborFinding(Id id) const;
  // find all neighbors contacting the left edge of tile `id` (bottom to up)
  std::vector<Id> LeftNeighborFinding(Id id) const;
  // find all neighbors contacting the top edge of tile `id` (right to left)
  std::vector<Id> TopNeighborFinding(Id id) const;
  // find all neighbors contacting the bottom edge of tile `id` (left to right)
  std::vector<Id> BottomNeighborFinding(Id id) const;
  // find the left-most-top solid tile in the given area
  Id AreaSearch(const Tile& area, Id start = kNullId) const;
  // enumerate all tiles in the given area,
  // each tile is visited after all its upper & left tiles are visited
  std::vector<Id> AreaEnum(const Tile& area, Id start = kNullId) const;

  Id InsertTile(Tile tile);
  Id DeleteTile(Tile tile);

#ifdef GTEST
 public:
#else
 protected:
#endif
  /* default: cover QuadrantI*/
  Pt coord_{0, 0};             // lower-left corner
  Pt size_{kLenMax, kLenMax};  // (width, height)
  std::vector<std::optional<Tile>> tiles_{Tile(Pt(0, 0), Pt(kLenMax, kLenMax))};
  std::stack<size_t> slots_{};  // index in tiles_ which are already deleted
  Id last_inserted_{kNullId};   // record last tile for better locality

  // get reference of tile `id` (without check)
  const Tile& Ref(Id id) const { return tiles_[id].value(); }
  Tile& Ref(Id id) { return tiles_[id].value(); }

  bool Exist(Id id) const {
    return id != kNullId && 0 <= id && (size_t)id < tiles_.size() &&
           tiles_[id].has_value();
  }
  Id LastInserted() const;
  // the recursive R procedure called by AreaEnum
  void AreaEnumHelper(const Tile& area, std::vector<Id>& enums, Id id) const;
  // allocate a new tile, return its id
  Id AllocTile();
  // free tile `id`, return 0 if success else return 1
  int FreeTile(Id id);
  // vertically split tile `id` along line x=`x`, return the created new tile
  Id VerticalSplit(Id id, Len x);
  // horizontally split tile `id` along line y=`y`, return the created new tile
  Id HorizontalSplit(Id id, Len y);
  // vertically merge tiles `left` & `right`, return the merged tile
  Id VerticalMerge(Id left, Id right);
  // horizontally merge tiles `lower` & `upper`, return the merged tile
  Id HorizontalMerge(Id lower, Id upper);
};
