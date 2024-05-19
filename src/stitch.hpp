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

  Id PointFinding(const Pt&);  // find the tile at (x,y)
  void NeighborFinding(const Tile& tile);
  void AreaSearch(const Pt& bl, const Pt& tr);
  Tile& InsertTile(Tile Tile);
  Tile& DeleteTile(Tile Tile);

 protected:
  /* default: cover QuadrantI*/
  Pt coord_{0, 0};             // lower-left corner
  Pt size_{kLenMax, kLenMax};  // (width, height)
  std::vector<std::optional<Tile>> tiles_{Tile(Pt(0, 0), Pt(kLenMax, kLenMax))};
  std::stack<size_t> slots_{};  // index in tiles_ which are already deleted

  Id FirstTile() const;

#ifdef GTEST
 public:
  void SetTiles(std::vector<std::optional<Tile>> tiles) {
    tiles_ = std::move(tiles);
  }
  void SetSlots(std::stack<size_t> slots) { slots_ = std::move(slots); }
#endif
};
