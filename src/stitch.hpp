#pragma once

#include <optional>
#include <stack>
#include <vector>

#include "tile.hpp"

class Stitch {
 public:
  Stitch() = default;
  Stitch(const Stitch& stitch) = default;
  Stitch(const Pt& size);

  size_t Size() const;

  Id PointFinding(const Pt&);  // find the tile at (x,y)
  void NeighborFinding(const Tile& tile);
  void AreaSearch(const Pt& bl, const Pt& tr);
  Tile& InsertTile(Tile Tile);
  Tile& DeleteTile(Tile Tile);

 protected:
  std::vector<Tile> tiles_;
  std::stack<size_t> slots_;  // index in tiles_ which are already deleted

  Id FirstTile() const;

#ifdef GTEST
  void SetTiles(std::vector<Tile> tiles) { tiles_ = tiles; }
  void SetDelTiles(std::stack<size_t> slots) { slots_ = slots; }
#endif
};
