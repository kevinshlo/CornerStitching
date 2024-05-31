#ifdef PY

#include "py.hpp"

#ifdef GTEST
#include "../test/pytest.hpp"
#define MODULE_NAME _stitch_pytest
#else
#define MODULE_NAME _stitch
#endif

namespace py = pybind11;
PYBIND11_MODULE(MODULE_NAME, m) {
  py::class_<Pt>(m, "Pt")
      .def_readonly("x", &Pt::x)
      .def_readonly("y", &Pt::y)
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
      .def(py::self == py::self)
      .def(py::self != py::self);

  py::class_<PyTile>(m, "Tile")
      .def(py::init<const PyTile&>())
      .def_property_readonly("exist", &PyTile::Exist)
      .def_property_readonly("coord", &PyTile::Coord)
      .def_property_readonly("size", &PyTile::Size)
      .def_property_readonly("is_space", &PyTile::IsSpace)
      .def_property_readonly("bl", &PyTile::BL)
      .def_property_readonly("lb", &PyTile::LB)
      .def_property_readonly("tr", &PyTile::TR)
      .def_property_readonly("rt", &PyTile::RT)
      .def("right_neighbors", &PyTile::RightNeighborFinding)
      .def("left_neighbors", &PyTile::LeftNeighborFinding)
      .def("top_neighbors", &PyTile::TopNeighborFinding)
      .def("bottom_neighbors", &PyTile::BottomNeighborFinding)
      .def("delete", &PyTile::Delete);

  py::class_<PyStitch>(m, "Stitch")
      .def(py::init<const PyStitch&>())
      .def(py::init<const Pt&, const Pt&>())
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

#ifdef GTEST
  m.def("pytest_tiles", &Tiles);
  m.def("pytest_golden_left_neighbors", &GoldenLeftNeighbors);
  m.def("pytest_golden_bottom_neighbors", &GoldenBottomNeighbors);
  m.def("pytest_golden_right_neighbors", &GoldenRightNeighbors);
  m.def("pytest_golden_top_neighbors", &GoldenTopNeighbors);
  m.def("pytest_left_neighbors", &LeftNeighbors);
  m.def("pytest_bottom_neighbors", &BottomNeighbors);
  m.def("pytest_right_neighbors", &RightNeighbors);
  m.def("pytest_top_neighbors", &TopNeighbors);
  m.def("pytest_check_neighbors", &CheckNeighbors);
  m.def("pytest_check_tiles", &CheckTiles);
  m.def("pytest_check_strip", &CheckStrip);
#endif
}

#endif
