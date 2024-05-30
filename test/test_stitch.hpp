#include <gtest/gtest.h>

#include "../src/stitch.hpp"

enum Side {
  LEFT = 0,
  BOTTOM,
  RIGHT,
  TOP,
  LAST,  // dummy
};

struct TestStitch {
  typedef std::map<Id, std::vector<Id>> NeighborGolden;

  Stitch s;
  std::array<NeighborGolden, LAST> neighbor_golden;

  TestStitch() = delete;
  TestStitch(Stitch&& s, std::array<NeighborGolden, LAST>&& neighbor_golden);

  // get the existing tiles
  std::vector<Id> Tiles() const;
  // get golden `side` neighbors of tile `id` by visiting all tiles
  std::vector<Id> GoldenNeighbors(Id id, Side side) const;
  // get `side` neighbors of tile `id`
  std::vector<Id> Neighbors(Id id, Side side) const;
  // check neighbors & pointers of all tiles
  void CheckNeighbors() const;
  // check no overlap between tiles & whole plane is covered
  void CheckTiles() const;
  // check maximum horizontal strip property
  void CheckStrip() const;

  // test on each tile's corners, starting from default & 4 corners of the plane
  void TestPointFinding() const;
  // test on all tiles
  void TestNeighborFinding() const;
  // test on each tile with its own area
  void TestAreaSearch() const;
  // test on each tile with its own area
  void TestAreaEnum() const;
  // split all tiles then merge each split pair of tiles
  Stitch TestVerticalSplitMerge() const;
  // split all tiles then merge each split pair of tiles
  Stitch TestHorizontalSplitMerge() const;
  // insert each space tile with its own area
  Stitch TestInsert() const;
};
