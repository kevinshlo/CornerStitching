#ifdef PY

#pragma once

#include <pybind11/pybind11.h>
// <pybind11/pybind11.h> must be first
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "stitch.hpp"
#include "tile.hpp"

class PyTile {
 public:
  Stitch& s_;
  Id id_;
  std::optional<Tile> At() const { return s_.At(id_); }
  std::optional<Tile> At(Id id) const { return s_.At(id); }
  bool ExistId(Id id) const { return s_.Exist(id); }

 public:
  PyTile() = delete;
  PyTile(const PyTile& t) : s_(t.s_), id_(t.id_) {}
  PyTile(Stitch& s, Id id = kNullId) : s_(s), id_(id) {}

  bool Exist() const { return s_.Exist(id_); }
  std::optional<Pt> Coord() const {
    auto t = At();
    return t.has_value() ? std::optional<Pt>(t.value().coord) : std::nullopt;
  }
  std::optional<Pt> Size() const {
    auto t = At();
    return t.has_value() ? std::optional<Pt>(t.value().size) : std::nullopt;
  }
  std::optional<bool> IsSpace() const {
    auto t = At();
    return t.has_value() ? std::optional<bool>(t.value().is_space)
                         : std::nullopt;
  }

  typedef std::optional<PyTile> OptPyTile;
  OptPyTile BL() const {
    auto t = At();
    if (t.has_value() && ExistId(t.value().bl))
      return PyTile(s_, t.value().bl);
    else
      return std::nullopt;
  }
  OptPyTile LB() const {
    auto t = At();
    if (t.has_value() && ExistId((t.value().lb)))
      return PyTile(s_, t.value().lb);
    else
      return std::nullopt;
  }
  OptPyTile TR() const {
    auto t = At();
    if (t.has_value() && ExistId(t.value().tr))
      return PyTile(s_, t.value().tr);
    else
      return std::nullopt;
  }
  OptPyTile RT() const {
    auto t = At();
    if (t.has_value() && ExistId(t.value().rt))
      return PyTile(s_, t.value().rt);
    else
      return std::nullopt;
  }
  std::vector<PyTile> RightNeighborFinding() const {
    auto ids = s_.RightNeighborFinding(id_);
    std::vector<PyTile> ret;
    ret.reserve(ids.size());
    for (auto id : ids) ret.push_back(PyTile(s_, id));
    return ret;
  }
  std::vector<PyTile> LeftNeighborFinding() const {
    auto ids = s_.LeftNeighborFinding(id_);
    std::vector<PyTile> ret;
    ret.reserve(ids.size());
    for (auto id : ids) ret.push_back(PyTile(s_, id));
    return ret;
  }
  std::vector<PyTile> TopNeighborFinding() const {
    auto ids = s_.TopNeighborFinding(id_);
    std::vector<PyTile> ret;
    ret.reserve(ids.size());
    for (auto id : ids) ret.push_back(PyTile(s_, id));
    return ret;
  }
  std::vector<PyTile> BottomNeighborFinding() const {
    auto ids = s_.BottomNeighborFinding(id_);
    std::vector<PyTile> ret;
    ret.reserve(ids.size());
    for (auto id : ids) ret.push_back(PyTile(s_, id));
    return ret;
  }
  int Delete() { return s_.Delete(id_).has_value() ? 0 : 1; }
};

typedef std::pair<Len, Len> Len2;
typedef std::optional<PyTile> OptPyTile;

class PyStitch {
 public:
  Stitch s_;

 public:
  PyStitch() = delete;
  PyStitch(const PyStitch& s) : s_(s.s_) {}
  PyStitch(const Pt& coord, const Pt& size) : s_(coord, size) {}

  size_t NumTiles() const { return s_.NumTiles(); }

  OptPyTile PointFinding(const Pt& pt, const OptPyTile& start = std::nullopt) {
    Id id = start.has_value() ? s_.PointFinding(pt, start.value().id_)
                              : s_.PointFinding(pt);
    if (s_.Exist(id))
      return PyTile(s_, id);
    else
      return std::nullopt;
  }
  std::vector<PyTile> RightNeighborFinding(const PyTile& t) const {
    if (&t.s_ != &s_) return {};
    return t.RightNeighborFinding();
  }
  std::vector<PyTile> LeftNeighborFinding(const PyTile& t) const {
    if (&t.s_ != &s_) return {};
    return t.LeftNeighborFinding();
  }
  std::vector<PyTile> TopNeighborFinding(const PyTile& t) const {
    if (&t.s_ != &s_) return {};
    return t.TopNeighborFinding();
  }
  std::vector<PyTile> BottomNeighborFinding(const PyTile& t) const {
    if (&t.s_ != &s_) return {};
    return t.BottomNeighborFinding();
  }
  OptPyTile AreaSearch(const Pt& coord, const Pt& size,
                       const OptPyTile& start = std::nullopt) {
    if (!coord.InQuadrantI() || !size.IsSize()) return std::nullopt;
    Tile area(coord, size);
    Id id = start.has_value() ? s_.AreaSearch(area, start.value().id_)
                              : s_.AreaSearch(area);
    if (s_.Exist(id))
      return PyTile(s_, id);
    else
      return std::nullopt;
  }
  std::vector<PyTile> AreaEnum(const Pt& coord, const Pt& size,
                               const OptPyTile& start = std::nullopt) {
    if (!coord.InQuadrantI() || !size.IsSize()) return {};
    Tile area(coord, size);
    auto ids = start.has_value() ? s_.AreaEnum(area, start.value().id_)
                                 : s_.AreaEnum(area);
    std::vector<PyTile> ret;
    ret.reserve(ids.size());
    for (auto id : ids) ret.push_back(PyTile(s_, id));
    return ret;
  }
  OptPyTile Insert(const Pt& coord, const Pt& size) {
    if (!coord.InQuadrantI() || !size.IsSize()) return std::nullopt;
    Id id = s_.Insert({coord, size});
    if (s_.Exist(id))
      return PyTile(s_, id);
    else
      return std::nullopt;
  }
  int Delete(PyTile& dead) {
    if (&dead.s_ != &s_)
      return 1;
    else
      return dead.Delete();
  }
};

#endif
