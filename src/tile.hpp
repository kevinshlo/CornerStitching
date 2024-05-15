#pragma once

typedef double Len;

struct Pt {
  Len x{0}, y{0};
  Pt() = default;
  Pt(const Pt&) = default;
  Pt(Len x, Len y) : x(x), y(y) {}
  Pt operator+(const Pt& p) { return Pt(x + p.x, y + p.y); }
  Pt operator-(const Pt& p) { return Pt(x - p.x, y - p.y); }
  bool operator==(const Pt& p) { return x == p.x && y == p.y; }
  bool operator!=(const Pt& p) { return operator==(p); }
};

typedef int Id;
constexpr Id kNullId = -1;

struct Tile {
  Pt coord{};      // lower-left corner
  Pt size{};       // (width, height)
  Id bl{kNullId};  // bottom-most left
  Id lb{kNullId};  // left-most bottom
  Id tr{kNullId};  // top-most right
  Id rt{kNullId};  // right-most top

  Tile() = default;
  Tile(const Tile&) = default;
  Tile(const Pt& coord, const Pt& size) : coord(coord), size(size) {}

  bool Legal() const { return size.x > 0 && size.y > 0; }

  enum Cmp {
    LT = -1,  // lefter/lower then the tile for a dimension
    EQ = 0,   // contained inside
    GT = 1    // righter/higher
  };
  Cmp CmpX(Len x) {
    if (x < coord.x) return LT;
    if ((coord.x + size.x) <= x) return GT;
    return EQ;
  }
  Cmp CmpX(const Pt& p) { return CmpX(p.x); }
  Cmp CmpY(Len y) {
    if (y < coord.y) return LT;
    if ((coord.y + size.y) <= y) return GT;
    return EQ;
  }
  Cmp CmpY(const Pt& p) { return CmpY(p.y); }
  bool Contain(const Pt& p) { return CmpX(p.x) == EQ && CmpY(p.y) == EQ; }
};