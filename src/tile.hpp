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
  bool QuadrantI() const { return (x >= 0) && (y >= 0); }
  // represents legal width & height relative to `coord` (default:(0, 0))
  bool IsSize(const Pt& coord = {0, 0}) const;
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
  Cmp CmpX(Len x) const;
  Cmp CmpX(const Pt& p) const { return CmpX(p.x); }
  Cmp CmpY(Len y) const;
  Cmp CmpY(const Pt& p) const { return CmpY(p.y); }
  bool Contain(const Pt& p) const { return CmpX(p.x) == EQ && CmpY(p.y) == EQ; }
};
