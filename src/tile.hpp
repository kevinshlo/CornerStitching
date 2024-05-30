#pragma once

#include <cassert>
#include <iostream>
#include <limits>

typedef double Len;
const Len kLenMax = std::numeric_limits<Len>::max();

struct Pt {
  Len x{0}, y{0};
  Pt() = default;
  Pt(const Pt&) = default;
  Pt(Len x, Len y) : x(x), y(y) {}
  friend std::ostream& operator<<(std::ostream&, const Pt&);
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
    return (0 <= x) && (x <= (kLenMax - coord.x)) &&  //
           (0 <= y) && (y <= (kLenMax - coord.y));
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
  friend std::ostream& operator<<(std::ostream&, const Tile&);
  bool operator==(const Tile& t) const;
  bool operator!=(const Tile& t) const { return !operator==(t); }

  Pt LowerLeftCorner() const { return coord; }
  Pt LowerRightCorner() const { return coord + Pt(size.x, 0); }
  Pt UpperLeftCorner() const { return coord + Pt(0, size.y); }
  Pt UpperRightCorner() const { return coord + size; }

  enum Cmp {
    LT = -1,  // lefter/lower then the tile for a dimension
    EQ = 0,   // contained inside (opened right/upper)
    GT = 1    // righter/upper
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
  /* `t` lies inside this tile? */
  bool Contain(const Tile& t) const {
    return Contain(t.coord) && !t.Contain(coord + size);
  }

  /* `t` and this tile overlaps along x-axis? */
  bool OverlapX(const Tile& t) const {
    return (CmpX(t.coord) == EQ) || (t.CmpX(coord) == EQ);
  }
  /* `t` and this tile overlaps along y-axis? */
  bool OverlapY(const Tile& t) const {
    return (CmpY(t.coord) == EQ) || (t.CmpY(coord) == EQ);
  }
  /* `t` geometrically overlaps with this tile */
  bool Overlap(const Tile& t) const { return OverlapX(t) && OverlapY(t); }
  /* the vertical line at `coord` with length `l` overlaps? */
  bool OverlapVerticalLine(const Pt& coord, Len l) const {
    return Overlap({coord, {0, l}});
  }
  /* the horizontal line at `coord` with length `l` overlaps? */
  bool OverlapHorizontalLine(const Pt& coord, Len l) const {
    return Overlap({coord, {l, 0}});
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
