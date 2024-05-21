#pragma once

#include <cassert>
#include <limits>

typedef double Len;
const Len kLenMax = std::numeric_limits<Len>::max();

struct Pt {
  Len x{0}, y{0};
  Pt() = default;
  Pt(const Pt&) = default;
  Pt(Len x, Len y) : x(x), y(y) {}
  Pt operator+(const Pt& p) const { return Pt(x + p.x, y + p.y); }
  Pt operator-(const Pt& p) const { return Pt(x - p.x, y - p.y); }
  Pt operator-() const { return Pt(-x, -y); }
  Pt operator*(Len l) const { return Pt(x * l, y * l); }
  Pt operator/(Len l) const { return Pt(x / l, y / l); }
  bool operator==(const Pt& p) const { return (x == p.x) && (y == p.y); }
  // lies in first quadrant
  bool InQuadrantI() const { return (x >= 0) && (y >= 0); }
  // represents legal width & height relative to `coord` (default:(0, 0))
  bool IsSize(const Pt& coord = {0, 0}) const {
    return (0 < x) && (x <= (kLenMax - coord.x)) &&  //
           (0 < y) && (y <= (kLenMax - coord.y));
  }
};

typedef int Id;
const Id kNullId = -1;

struct Tile {
  Pt coord{};      // lower-left corner
  Pt size{};       // (width, height)
  Id bl{kNullId};  // bottom-most left
  Id lb{kNullId};  // left-most bottom
  Id tr{kNullId};  // top-most right
  Id rt{kNullId};  // right-most top
  bool is_space{true};

  Tile() = default;
  Tile(const Tile&) = default;
  Tile(const Pt& coord, const Pt& size, bool is_space = true);
  Tile(const Pt& coord, const Pt& size, Id bl, Id lb, Id tr, Id rt,
       bool is_space = true);

  enum Cmp {
    LT = -1,  // lefter/lower then the tile for a dimension
    EQ = 0,   // contained inside
    GT = 1    // righter/higher
  };
  /* relative position of `x` along x-axis to this tile */
  Cmp CmpX(Len x) const {
    return x < coord.x ? LT : (coord.x + size.x) <= x ? GT : EQ;
  }
  /* relative position of `p` along x-axis to this tile */
  Cmp CmpX(const Pt& p) const { return CmpX(p.x); }
  /* relative position of `y` along y-axis to this tile */
  Cmp CmpY(Len y) const {
    return y < coord.y ? LT : (coord.y + size.y) <= y ? GT : EQ;
  }
  /* relative position of `y` along y-axis to this tile */
  Cmp CmpY(const Pt& p) const { return CmpY(p.y); }
  /* `p` lies inside this tile? */
  bool Contain(const Pt& p) const { return CmpX(p.x) == EQ && CmpY(p.y) == EQ; }

  /* `t` and this tile overlaps along x-axis? */
  bool OverlapX(const Tile& t) const {
    return (CmpX(t.coord) != GT) && (CmpX(t.coord + t.size) != LT);
  }
  /* `t` and this tile overlaps along y-axis? */
  bool OverlapY(const Tile& t) const {
    return (CmpY(t.coord) != GT) && (CmpY(t.coord + t.size) != LT);
  }

  bool IsRightNeighborTo(const Tile& t) const {
    return (coord.x == (t.coord.x + t.size.x)) && OverlapY(t);
  }
  bool IsLeftNeighborTo(const Tile& t) const {
    return ((coord.x + size.x) == t.coord.x) && OverlapY(t);
  }
  bool IsTopNeighborTo(const Tile& t) const {
    return (coord.y == (t.coord.y + t.size.y)) && OverlapX(t);
  }
  bool IsBottomNeighborTo(const Tile& t) const {
    return ((coord.y + size.y) == t.coord.y) && OverlapX(t);
  }
};
