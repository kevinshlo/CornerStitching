#ifdef PY

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "stitch.hpp"
#include "tile.hpp"

class PyTile {
 public:
  const Stitch& s_;
  Id id_;
  std::optional<Tile> At() const { return s_.At(id_); }
  std::optional<Tile> At(Id id) const { return s_.At(id); }
  bool ExistId(Id id) const { return s_.Exist(id); }

 public:
  PyTile() = delete;
  PyTile(const PyTile& t) : s_(t.s_), id_(t.id_) {}
  PyTile(const Stitch& s, Id id = kNullId) : s_(s), id_(id) {}

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
  PyStitch(const Len2& coord, const Len2& size) : s_(coord, size) {}

  size_t NumTiles() const { return s_.NumTiles(); }

  OptPyTile PointFinding(const Pt& pt,
                         const OptPyTile& start = std::nullopt) const {
    Id id = start.has_value() ? s_.PointFinding(pt, start.value().id_)
                              : s_.PointFinding(pt);
    if (s_.Exist(id))
      return PyTile(s_, id);
    else
      return std::nullopt;
  }
  std::vector<PyTile> RightNeighborFinding(const PyTile& t) const {
    auto ids = s_.RightNeighborFinding(t.id_);
    std::vector<PyTile> ret;
    ret.reserve(ids.size());
    for (auto id : ids) ret.push_back(PyTile(s_, id));
    return ret;
  }
  std::vector<PyTile> LeftNeighborFinding(const PyTile& t) const {
    auto ids = s_.LeftNeighborFinding(t.id_);
    std::vector<PyTile> ret;
    ret.reserve(ids.size());
    for (auto id : ids) ret.push_back(PyTile(s_, id));
    return ret;
  }
  std::vector<PyTile> TopNeighborFinding(const PyTile& t) const {
    auto ids = s_.TopNeighborFinding(t.id_);
    std::vector<PyTile> ret;
    ret.reserve(ids.size());
    for (auto id : ids) ret.push_back(PyTile(s_, id));
    return ret;
  }
  std::vector<PyTile> BottomNeighborFinding(const PyTile& t) const {
    auto ids = s_.BottomNeighborFinding(t.id_);
    std::vector<PyTile> ret;
    ret.reserve(ids.size());
    for (auto id : ids) ret.push_back(PyTile(s_, id));
    return ret;
  }
  OptPyTile AreaSearch(const Pt& coord, const Pt& size,
                       const OptPyTile& start = std::nullopt) const {
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
                               const OptPyTile& start = std::nullopt) const {
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
  int Delete(const PyTile& dead) {
    return s_.Delete(dead.id_).has_value() ? 0 : 1;
  }
};

namespace py = pybind11;
PYBIND11_MODULE(_stitch, m) {
  py::class_<Pt>(m, "Pt")
      .def(py::init<>())
      .def(py::init<const Pt&>())
      .def(py::init<Len, Len>())
      .def(py::init<const std::pair<Len, Len>&>())
      .def(py::self + py::self)
      .def(py::self - py::self)
      .def(-py::self)
      .def(
          "__mul__", [](Len l, const Pt& p) { return p * l; },
          py::is_operator())
      .def(py::self * Len())
      .def(py::self == py::self);
  py::class_<PyTile>(m, "Tile")
      .def(py::init<const PyTile&>())
      .def("exist", &PyTile::Exist)
      .def("coord", &PyTile::Coord)
      .def("size", &PyTile::Size)
      .def("is_space", &PyTile::IsSpace)
      .def("bl", &PyTile::BL)
      .def("lb", &PyTile::LB)
      .def("tr", &PyTile::TR)
      .def("rt", &PyTile::RT);
  py::class_<PyStitch>(m, "Stitch")
      .def(py::init<const PyStitch&>())
      .def(py::init<const Pt&, const Pt&>())
      .def(py::init<const Len2&, const Len2&>())
      .def("__len__", &PyStitch::NumTiles)
      .def("pt_find", &PyStitch::PointFinding)
      .def("right_neighbors", &PyStitch::RightNeighborFinding)
      .def("left_neighbors", &PyStitch::LeftNeighborFinding)
      .def("top_neighbors", &PyStitch::TopNeighborFinding)
      .def("bottom_neighbors", &PyStitch::BottomNeighborFinding)
      .def("area_search", &PyStitch::AreaSearch)
      .def("area_enum", &PyStitch::AreaEnum)
      .def("insert", &PyStitch::Insert)
      .def("delete", &PyStitch::Delete);
}

#endif
