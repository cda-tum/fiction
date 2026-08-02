//
// Created by Jan Drewniok on 11.04.24.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb_nm_distance.hpp>

#include <fmt/format.h>

#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void nanometer_distance(nanobind::module_& m, const std::string& lattice = "")
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(fmt::format("sidb_nm_distance{}", lattice).c_str(), &fiction::sidb_nm_distance<Lyt>, py::arg("lyt"),
          py::arg("source"), py::arg("target"), DOC(fiction_sidb_nm_distance));
}

}  // namespace detail

void sidb_nm_distance(nanobind::module_& m)
{
    detail::nanometer_distance<py_sidb_100_lattice>(m, "_100");
    detail::nanometer_distance<py_sidb_111_lattice>(m, "_111");
}

}  // namespace pyfiction
