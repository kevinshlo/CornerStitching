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

  size_t Size() const { return tiles_.size() - slots_.size(); }

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
  void AreaSearch(const Pt& bl, const Pt& tr);
  Tile& InsertTile(Tile Tile);
  Tile& DeleteTile(Tile Tile);

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

  bool Exist(Id id) const { return id != kNullId && tiles_[id].has_value(); }
  Id LastInserted() const;
};
