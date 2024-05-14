#ifdef PY

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "stitch.hpp"
#include "tile.hpp"

namespace py = pybind11;
PYBIND11_MODULE(_stitch, m) {
  py::class_<Stitch>(m, "Stitch");
  py::class_<Stitch>(m, "Stitch");
}

#endif
